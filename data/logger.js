// ---------------------------------------------------------------------------
// logger.js - Logika za upravljanje dnevnikom radio veza (Logger)
// ---------------------------------------------------------------------------
// Upravlja unosom QSO-ova, validacijom pozivnih oznaka, automatskim
// prepoznavanjem država (DXCC), filtriranjem tablice i izvozom podataka.
// ---------------------------------------------------------------------------

let radioLogs = [];
let isLoadingLogs = false; 
let currentSearchTerm = ''; 
let dxccData = []; 
let workedCountries = new Set(); 

document.addEventListener('DOMContentLoaded', () => {
    const path = window.location.pathname;
    const normalizedPath = path.endsWith('/') ? path.slice(0, -1) : path.replace('.html', '');

    if (normalizedPath === '/logger') {
        onLoadLogger();
    }

    if (typeof updateConnectionStatus === 'function') {
        updateConnectionStatus('connected'); 
    } else {
        console.warn("Funkcija updateConnectionStatus nije pronađena. Provjerite common.js.");
    }
});

// ---------------------------------------------------------------------------
// Funkcija koja se poziva pri učitavanju logger stranice
// ---------------------------------------------------------------------------
async function onLoadLogger() {
    if (typeof getUtcTime === 'undefined' || typeof sendHttpRequest === 'undefined' || typeof showFormMessage === 'undefined' || typeof downloadFile === 'undefined') {
        console.error("Neke od očekivanih funkcija iz common.js nisu definirane. Provjerite da li je common.js ispravno učitan.");
        showFormMessage('Greška: Neki dijelovi aplikacije neće raditi ispravno. Provjerite common.js.', 'error', 0); 
        return; 
    }

    await getUtcTime();
    await getLogs(); 
    await loadDxccData();



    setInterval(getUtcTime, 1000); 

    const logForm = document.getElementById('logForm');
    if (logForm) {
        logForm.addEventListener('submit', function(e) {
            e.preventDefault();
            addLogEntry();
        });
    }

    const callsignInput = document.getElementById('callsign');
    if (callsignInput) {
    callsignInput.addEventListener('input', (e) => {
        handleCallsignRealtime(e.target.value);
    });
}

    const searchInput = document.getElementById('searchInput');
    if (searchInput) {
        let searchTimeout;
        searchInput.addEventListener('keyup', () => {
            clearTimeout(searchTimeout);
            searchTimeout = setTimeout(() => {
                currentSearchTerm = searchInput.value.toLowerCase().trim(); 
                filterLogs();
            }, 300); 
        });
    }

    const clearAllLogsBtn = document.querySelector('.table-footer .button.tertiary[aria-label="Obriši sve logove"]');
    if (clearAllLogsBtn) {
        clearAllLogsBtn.addEventListener('click', confirmClearAllLogs);
    }

    const exportADIFBtn = document.querySelector('.table-footer .button.secondary[aria-label="Izvezi sve logove u ADIF format"]');
    if (exportADIFBtn) {
        exportADIFBtn.addEventListener('click', exportToADIF);
    }

    const exportCSVBtn = document.querySelector('.table-footer .button[aria-label="Izvezi sve logove u CSV format"]');
    if (exportCSVBtn) {
        exportCSVBtn.addEventListener('click', exportToCSV);
    }

    const modulationSelect = document.getElementById('modulation');
    if (modulationSelect) {
        modulationSelect.addEventListener('change', updateRSTDefault);
        updateRSTDefault(); 
    }

    const refreshLogsButton = document.getElementById('refreshLogsButton'); 
    if (refreshLogsButton) {
        refreshLogsButton.addEventListener('click', async () => {
            currentSearchTerm = ''; 
            if (searchInput) searchInput.value = ''; 
            await getLogs(); 
        });
    }

    const contestModeToggle = document.getElementById('contestModeToggle');
    if (contestModeToggle) {
        contestModeToggle.addEventListener('change', (e) => {
            const isContest = e.target.checked;
            const contestFields = document.querySelectorAll('.contest-only');
            // Koristimo flex jer je roditelj .input-with-nr fleksibilan
            contestFields.forEach(f => f.style.display = isContest ? 'flex' : 'none');
            
            if (isContest) {
                updateContestSerial();
            }
        });
    }
}

// ---------------------------------------------------------------------------
// Dohvaća UTC vrijeme s ESP-a i ažurira prikaz
// ---------------------------------------------------------------------------
async function getUtcTime() {
    try {
        const data = await sendHttpRequest('/api/utc_time'); 
        const utcTimeElement = document.getElementById('utcTime');
        
        if (utcTimeElement) {
            const timeStr = data.time || data.utc_time || 'N/A';
            utcTimeElement.innerText = timeStr + " UTC";
            utcTimeElement.setAttribute('datetime', timeStr);
        }
    } catch (e) {
        console.error("Greška pri dohvaćanju UTC vremena:", e);
        const utcTimeElement = document.getElementById('utcTime');
        if (utcTimeElement) utcTimeElement.innerText = "Veza prekinuta";
    }
}

// ---------------------------------------------------------------------------
// Funkcija za slanje novog loga na ESP putem HTTP POST-a
// ---------------------------------------------------------------------------
async function addLogEntry() {
    const form = document.getElementById('logForm');
    if (!form) return;

    const callsignInput = document.getElementById('callsign');
    const callsign = callsignInput.value.trim().toUpperCase();
    const callsignPattern = /^[A-Z0-9\/]{3,15}$/;

    if (!callsignPattern.test(callsign)) {
        showFormMessage('Molimo unesite valjanu pozivnu oznaku (npr. 9A1AA, F/9A5AGN/P). Minimalno 3 znaka, dozvoljeni su brojevi i "/". Maksimalno 15 znakova.', 'error');
        callsignInput.focus();
        return;
    }

    if (!form.checkValidity()) {
        form.reportValidity();
        showFormMessage('Molimo popunite sva obavezna polja ispravno.', 'error');
        return;
    }

    let frequencyValue = document.getElementById('frequency').value;
    const frequency = isNaN(parseFloat(frequencyValue)) ? 0.0 : parseFloat(frequencyValue);
    
    const sentNrInput = document.getElementById('sentNr');
    const rcvdNrInput = document.getElementById('rcvdNr');
    const isContest = document.getElementById('contestModeToggle').checked;

    const rstSent = document.getElementById('rstSent').value.trim(); 
    const rstReceived = document.getElementById('rstReceived').value.trim(); 
    const qthlocator = document.getElementById('qthlocator').value.trim().toUpperCase();
    const notes = document.getElementById('notes').value.trim();
    const name = document.getElementById('name').value.trim(); 
    const modulation = document.getElementById('modulation').value;
    
    const utcTimeElement = document.getElementById('utcTime');
    const timestamp = utcTimeElement ? utcTimeElement.textContent.split(' UTC')[0].trim() : new Date().toISOString().slice(0, 19).replace('T', ' ');

    const newLog = {
        time: timestamp,
        callsign: callsign,
        country: document.getElementById('callsign').dataset.currentCountry || "Unknown",
        name: name,
        frequency: frequency,
        modulation: modulation,
        rstSent: rstSent, 
        rstReceived: rstReceived, 
        sentNr: isContest ? sentNrInput.value : "",
        rcvdNr: isContest ? rcvdNrInput.value : "",
        qthlocator: qthlocator,
        notes: notes
    };

    try {
        await sendHttpRequest('/save_log', 'POST', newLog);
        
        const oldCall = callsign;
        form.reset();
        
        callsignInput.setCustomValidity('');
        showFormMessage(`Log za ${oldCall} uspješno spremljen!`, 'success');
        
        await getLogs(); 

        if (isContest) {
            document.getElementById('contestModeToggle').checked = true;
            document.querySelectorAll('.contest-only').forEach(f => f.style.display = 'flex');
            updateContestSerial();
        }
    } catch (e) {
        console.error("Greška pri spremanju loga:", e);
        showFormMessage('Greška pri spremanju loga. Provjerite konzolu za više detalja.', 'error');
    }
}

// ---------------------------------------------------------------------------
// Prikazuje logove u tablici
// ---------------------------------------------------------------------------
function displayLogs(logs) {
    const tbody = document.querySelector('#logTable tbody');
    const noLogsMessage = document.getElementById('noLogsMessage');
    const exportADIFBtn = document.getElementById('exportADIFBtn');
    const clearAllLogsBtn = document.getElementById('clearLogsBtn');
    const exportCSVBtn = document.getElementById('exportCSVBtn');

    if (!tbody || !noLogsMessage) return;

    radioLogs = logs;
    filterLogs(false);
}

// ---------------------------------------------------------------------------
// Dohvaća sve logove s ESP uređaja
// ---------------------------------------------------------------------------
async function getLogs() {
    if (isLoadingLogs) return;

    isLoadingLogs = true;
    showLoadingIndicator(true);

    const noLogsMessage = document.getElementById('noLogsMessage');
    const exportADIFBtn = document.getElementById('exportADIFBtn');
    const clearAllLogsBtn = document.getElementById('clearLogsBtn');
    const exportCSVBtn = document.getElementById('exportCSVBtn');
    const exportEDIBtn = document.getElementById('exportEDIBtn');

    try {
        const data = await sendHttpRequest('/logs');
        const logsToDisplay = Array.isArray(data) ? data : (data.logs || data.logovi || []);
        
        radioLogs = logsToDisplay;
        filterLogs();

        if (radioLogs.length === 0) {
            noLogsMessage.classList.remove('hidden');
            if (exportADIFBtn) exportADIFBtn.style.display = 'none';
            if (clearAllLogsBtn) clearAllLogsBtn.style.display = 'none';
            if (exportCSVBtn) exportCSVBtn.style.display = 'none';
            if (exportEDIBtn) exportEDIBtn.style.display = 'none';
        } else {
            noLogsMessage.classList.add('hidden');
            if (exportADIFBtn) exportADIFBtn.style.display = 'inline-block';
            if (clearAllLogsBtn) clearAllLogsBtn.style.display = 'inline-block';
            if (exportCSVBtn) exportCSVBtn.style.display = 'inline-block';
            if (exportEDIBtn) exportEDIBtn.style.display = 'inline-block';
        }

    } catch (e) {
        console.error("Greška pri dohvaćanju logova:", e);
        showFormMessage("Greška pri dohvaćanju logova. Provjerite konzolu.", 'error');
        document.querySelector('#logTable tbody').innerHTML = '';
        if (noLogsMessage) noLogsMessage.classList.remove('hidden');
        if (exportADIFBtn) exportADIFBtn.style.display = 'none';
        if (clearAllLogsBtn) clearAllLogsBtn.style.display = 'none';
        if (exportCSVBtn) exportCSVBtn.style.display = 'none';
    } finally {
        isLoadingLogs = false;
        showLoadingIndicator(false);
    }

    workedCountries.clear();
    radioLogs.forEach(log => {
        if (log.country) workedCountries.add(log.country);
    });
}

// ---------------------------------------------------------------------------
// Filtriranje logova na klijentskoj strani (pretraga)
// ---------------------------------------------------------------------------
function filterLogs() {
    const tbody = document.querySelector('#logTable tbody');
    const noLogsMessage = document.getElementById('noLogsMessage');
    if (!tbody || !noLogsMessage) return;

    tbody.innerHTML = '';

    const searchTerm = currentSearchTerm;

    const filtered = radioLogs.filter(log => {
        return (log.callsign && log.callsign.toLowerCase().includes(searchTerm)) ||
               (log.name && log.name.toLowerCase().includes(searchTerm)) || 
               (log.qthlocator && log.qthlocator.toLowerCase().includes(searchTerm)) ||
               (log.notes && log.notes.toLowerCase().includes(searchTerm)) ||
               (log.time && log.time.toLowerCase().includes(searchTerm)) ||
               (log.frequency && log.frequency.toString().includes(searchTerm)) ||
               (log.modulation && log.modulation.toLowerCase().includes(searchTerm)) ||
               (log.rstSent && log.rstSent.toLowerCase().includes(searchTerm)) || 
               (log.rstReceived && log.rstReceived.toLowerCase().includes(searchTerm));
    });

    if (filtered.length === 0) {
        noLogsMessage.classList.remove('hidden');
    } else {
        noLogsMessage.classList.add('hidden');
        const fragment = document.createDocumentFragment();

        filtered.forEach((log) => {
            const row = document.createElement('tr');
            const originalIndex = radioLogs.findIndex(item =>
                item.time === log.time && item.callsign === log.callsign && item.frequency === log.frequency
            );
            row.dataset.originalIndex = originalIndex;

            const cellTime = row.insertCell();
            cellTime.textContent = log.time || 'N/A';
            cellTime.setAttribute('data-label', 'Vrijeme (UTC)');

            const cellCallsign = row.insertCell();
            cellCallsign.textContent = log.callsign || 'N/A';
            cellCallsign.setAttribute('data-label', 'Pozivna');

            const cellCountry = row.insertCell(); 
            cellCountry.textContent = log.country || 'N/A';
            cellCountry.setAttribute('data-label', 'Država');

            const cellName = row.insertCell();
            cellName.textContent = log.name || ''; 
            cellName.setAttribute('data-label', 'Ime');

            const cellFreq = row.insertCell();
            cellFreq.textContent = log.frequency ? `${parseFloat(log.frequency).toFixed(3)} MHz` : 'N/A';
            cellFreq.setAttribute('data-label', 'Frekvencija');

            const cellMod = row.insertCell();
            cellMod.textContent = log.modulation || 'N/A';
            cellMod.setAttribute('data-label', 'Modulacija');

            const cellRST = row.insertCell();
            let rstDisplay = `${log.rstSent || ''}/${log.rstReceived || ''}`;
            if (log.sentNr || log.rcvdNr) {
                rstDisplay += ` (${log.sentNr || '-'}/${log.rcvdNr || '-'})`;
            }
            cellRST.textContent = rstDisplay;
            cellRST.setAttribute('data-label', 'RST (Nr)');

            const cellQTH = row.insertCell();
            cellQTH.textContent = log.qthlocator || 'N/A';
            cellQTH.setAttribute('data-label', 'QTH');

            const cellNotes = row.insertCell();
            cellNotes.textContent = log.notes || '';
            cellNotes.setAttribute('data-label', 'Bilješke');

            const actionsCell = row.insertCell();
            actionsCell.setAttribute('data-label', 'Akcije');

            const deleteButton = document.createElement('button');
            deleteButton.classList.add('button', 'small', 'danger');
            deleteButton.innerHTML = '<i class="fas fa-trash"></i>';
            deleteButton.title = `Obriši log za ${log.callsign || 'nepoznatu pozivnu oznaku'}`;
            deleteButton.onclick = () => confirmDeleteLog(originalIndex);
            actionsCell.appendChild(deleteButton);

            fragment.appendChild(row);
        });
        tbody.appendChild(fragment);
    }
}

// ---------------------------------------------------------------------------
// Prikazuje ili skriva loading indikator za logove
// ---------------------------------------------------------------------------
function showLoadingIndicator(show) {
    const indicator = document.getElementById('logLoadingIndicator');
    if (indicator) {
        indicator.style.display = show ? 'block' : 'none';
    }
}


// ---------------------------------------------------------------------------
// Brisanje pojedinačnog loga s potvrdom korisnika
// ---------------------------------------------------------------------------
async function confirmDeleteLog(index) {
    if (index >= 0 && index < radioLogs.length) {
        if (confirm(`Jeste li sigurni da želite obrisati log za "${radioLogs[index].callsign || 'ovu pozivnu oznaku'}"?`)) {
            try {
                await sendHttpRequest(`/delete_log?index=${index}`, 'GET'); 
                showFormMessage('Log uspješno obrisan!', 'success');
                await getLogs();
            } catch (e) {
                console.error("Greška pri brisanju loga:", e);
                showFormMessage("Greška pri brisanju loga. Provjerite konzolu.", 'error');
            }
        }
    } else {
        console.warn("Pokušaj brisanja loga s nevažećim indeksom:", index);
        showFormMessage("Greška: Nevažeći indeks loga za brisanje.", 'error');
    }
}

// ---------------------------------------------------------------------------
// Potvrda i brisanje svih logova iz memorije uređaja
// ---------------------------------------------------------------------------
async function confirmClearAllLogs() {
    if (radioLogs.length === 0) {
        showFormMessage('Nema logova za brisanje.', 'warning');
        return;
    }
    if (confirm("Jeste li sigurni da želite obrisati SVE radio logove? Ova radnja je nepovratna!")) {
        try {
            await sendHttpRequest('/clear_all_logs', 'GET'); 
            showFormMessage('Svi logovi su uspješno obrisani!', 'success');
            await getLogs();
        } catch (e) {
            console.error("Greška pri brisanju svih logova:", e);
            showFormMessage("Greška pri brisanju svih logova. Provjerite konzolu.", 'error');
        }
    }
}

// ---------------------------------------------------------------------------
// Izvoz logova u standardni ADIF format
// ---------------------------------------------------------------------------
function exportToADIF() {
    if (radioLogs.length === 0) {
        showFormMessage('Nema logova za izvoz u ADIF format.', 'info');
        return;
    }

    let adifContent = "";
    adifContent += "ADIF Exported by 9A5AGN Radio Logger\n";
    adifContent += "<adif_ver:5>3.1.4 <programid:12>Radio Logger <eoh>\n\n";

    radioLogs.forEach(log => {
        const callsign = log.callsign || "";
        const name = log.name || "";
        const frequency = log.frequency ? parseFloat(log.frequency).toFixed(3) : "";
        const modulation = log.modulation || "";
        const rstSent = log.rstSent || "";
        const rstReceived = log.rstReceived || "";
        const qthlocator = log.qthlocator || "";
        const notes = log.notes || "";
        const timeOn = log.time ? log.time.substring(11, 16).replace(/:/g, '') : "";
        const qsoDate = log.time ? log.time.substring(0, 10).replace(/-/g, '') : "";

        adifContent += `<CALL:${callsign.length}>${callsign}\n`;
        if (name) adifContent += `<NAME:${name.length}>${name}\n`;
        adifContent += `<FREQ:${frequency.length}>${frequency}\n`;
        adifContent += `<MODE:${modulation.length}>${modulation}\n`;
        adifContent += `<RST_SENT:${rstSent.length}>${rstSent}\n`;
        adifContent += `<RST_RCVD:${rstReceived.length}>${rstReceived}\n`;
        adifContent += `<QSO_DATE:${qsoDate.length}>${qsoDate}\n`;
        adifContent += `<TIME_ON:${timeOn.length}>${timeOn}\n`;
        if (qthlocator) {
            adifContent += `<GRIDSQUARE:${qthlocator.length}>${qthlocator}\n`;
        }

        if (notes) {
            adifContent += `<COMMENT:${notes.length}>${notes}\n`;
        }
        adifContent += `<EOR>\n`;
    });

    downloadFile(adifContent, `radio_logs_${new Date().toISOString().slice(0, 10)}.adif`, "application/adif");
    showFormMessage('Logovi izvezeni u ADIF format!', 'success');
}

// ---------------------------------------------------------------------------
// Izvoz logova u CSV format (Excel kompatibilno)
// ---------------------------------------------------------------------------
function exportToCSV() {
    if (radioLogs.length === 0) {
        showFormMessage('Nema logova za izvoz u CSV format.', 'info');
        return;
    }

    let csvContent = "Vrijeme UTC,Pozivna Oznaka,Ime,Frekvencija (MHz),Modulacija,RST Poslano,RST Primljeno,QTH Lokator,Bilješke\n";

    radioLogs.forEach(log => {
        const time = `"${log.time || ''}"`;
        const callsign = `"${(log.callsign || '').replace(/"/g, '""')}"`;
        const name = `"${(log.name || '').replace(/"/g, '""')}"`;
        const frequency = `"${log.frequency || ''}"`;
        const modulation = `"${(log.modulation || '').replace(/"/g, '""')}"`;
        const rstSent = `"${(log.rstSent || '').replace(/"/g, '""')}"`;
        const rstReceived = `"${(log.rstReceived || '').replace(/"/g, '""')}"`;
        const qthlocator = `"${(log.qthlocator || '').replace(/"/g, '""')}"`;
        const notes = `"${(log.notes || '').replace(/"/g, '""')}"`;

        csvContent += `${time},${callsign},${name},${frequency},${modulation},${rstSent},${rstReceived},${qthlocator},${notes}\n`;
    });

    downloadFile(csvContent, `radio_logs_${new Date().toISOString().slice(0, 10)}.csv`, "text/csv");
    showFormMessage('Logovi izvezeni u CSV format!', 'success');
}

// ---------------------------------------------------------------------------
// Izvoz logova u EDI format - preusmjeravanje na konfiguraciju
// ---------------------------------------------------------------------------
function exportToEDI() {
    if (radioLogs.length === 0) {
        showFormMessage('Nema logova za izvoz u EDI format.', 'info');
        return;
    }
    sessionStorage.setItem('currentLogs', JSON.stringify(radioLogs));
    window.location.href = 'edi_export.html';
}


function updateRSTDefault() {
    const modulation = document.getElementById('modulation').value;
    const rstSentInput = document.getElementById('rstSent');
    const rstReceivedInput = document.getElementById('rstReceived');

    if (!rstSentInput || !rstReceivedInput) {
        return;
    }

    if (modulation === 'CW') {
        rstSentInput.value = '599';
        rstReceivedInput.value = '599';
        rstSentInput.pattern = "^[1-5][0-9][0-9]$"; 
        rstReceivedInput.pattern = "^[1-5][0-9][0-9]$"; 
        rstSentInput.title = "Unesite valjan RST (npr. 599).";
        rstReceivedInput.title = "Unesite valjan RST (npr. 599).";
    } else if (modulation === 'FT8' || modulation === 'FT4' || modulation === 'DIGITAL' || modulation === 'RTTY') {
        rstSentInput.value = '-10'; 
        rstReceivedInput.value = '-10'; 
        rstSentInput.pattern = "^[+-]?[0-9]{1,2}$"; 
        rstReceivedInput.pattern = "^[+-]?[0-9]{1,2}$"; 
        rstSentInput.title = "Unesite valjan RST (npr. -10, +05).";
        rstReceivedInput.title = "Unesite valjan RST (npr. -10, +05).";
    } else { 
        rstSentInput.value = '59';
        rstReceivedInput.value = '59';
        rstSentInput.pattern = "^[1-5][0-9]$"; 
        rstReceivedInput.pattern = "^[1-5][0-9]$"; 
        rstSentInput.title = "Unesite valjan RST (npr. 59).";
        rstReceivedInput.title = "Unesite valjan RST (npr. 59).";
    }
}

// ---------------------------------------------------------------------------
// Učitava bazu DXCC prefiksa iz prefixes.json datoteke
// ---------------------------------------------------------------------------
async function loadDxccData() {
        const data = await sendHttpRequest('/prefixes.json');
        
        if (Array.isArray(data)) {
            dxccData = data;
            

            dxccData.sort((a, b) => {
                const lenA = (a && a.prefix) ? a.prefix.length : 0;
                const lenB = (b && b.prefix) ? b.prefix.length : 0;
                return lenB - lenA;
            });
            console.log("DXCC podaci učitani. Broj prefiksa:", dxccData.length);
        } else {
            console.error("Format prefixes.json nije ispravan (očekuje se niz).");
        }

}

// ---------------------------------------------------------------------------
// Prepoznavanje države i dupe check u stvarnom vremenu
// ---------------------------------------------------------------------------
function handleCallsignRealtime(rawCall) {
    const call = rawCall.trim().toUpperCase();
    const flagImg = document.getElementById('flagImg');
    const countryName = document.getElementById('countryName');
    const callsignInput = document.getElementById('callsign');

    if (call.length < 1) {
        if (flagImg) flagImg.style.display = 'none';
        if (countryName) countryName.innerHTML = "";
        if (callsignInput) delete callsignInput.dataset.currentCountry;
        return;
    }

    let prefixToSearch = call;
    if (call.includes('/')) {
        let parts = call.split('/');
        prefixToSearch = (parts[0].length <= 3) ? parts[0] : parts[1];
    }

    let found = dxccData.find(item => item.prefix && prefixToSearch.startsWith(item.prefix));

    const isCallsignWorked = radioLogs.some(log => log.callsign && log.callsign.toUpperCase() === call);

    if (found) {
        const isCountryWorked = workedCountries.has(found.name);
        
        let status = " (NEW ONE!)";
        let color = "#ff4444"; 

        if (isCallsignWorked) {
            status = " (Odrađeno - Pozivna)";
            color = "#00C851"; 
        } else if (isCountryWorked) {
            status = " (Odrađeno - Država)";
            color = "#ffbb33"; 
        }
        
        if (flagImg && countryName) {
            flagImg.src = `https://flagcdn.com/w40/${found.flag}.png`;
            flagImg.style.display = 'inline-block';
            countryName.style.color = color;
            countryName.innerHTML = `${found.name} <strong>${status}</strong>`;
        }
        
        if (callsignInput) {
            callsignInput.dataset.currentCountry = found.name;
        }
    } else {
        if (isCallsignWorked) {
            if (flagImg) flagImg.style.display = 'none';
            if (countryName) {
                countryName.style.color = '#00C851';
                countryName.innerHTML = `Nepoznat prefiks <strong>(Odrađeno - Pozivna)</strong>`;
            }
            if (callsignInput) delete callsignInput.dataset.currentCountry;
        } else {
            if (flagImg) flagImg.style.display = 'none';
            if (countryName) {
                countryName.style.color = '#7f8c8d';
                countryName.innerHTML = `Nepoznat prefiks`;
            }
            if (callsignInput) delete callsignInput.dataset.currentCountry;
        }
    }
}

// ---------------------------------------------------------------------------
// Otvara QRZ.com stranicu s detaljima o pozivnoj oznaci
// ---------------------------------------------------------------------------
function openQrz() {
    const callsignInput = document.getElementById('callsign');
    
    if (callsignInput && callsignInput.value.trim() !== "") {
        const callsign = callsignInput.value.trim().toUpperCase();
        const url = `https://www.qrz.com/db/${callsign}`;
        
        window.open(url, '_blank');
    } else {
        alert("Molimo unesite pozivnu oznaku.");
    }
}

// ---------------------------------------------------------------------------
// Automatsko izračunavanje sljedećeg serijskog broja za natjecanja
// ---------------------------------------------------------------------------
function updateContestSerial() {
    const sentNrInput = document.getElementById('sentNr');
    if (!sentNrInput) return;
    let maxNr = 0;
    if (radioLogs && radioLogs.length > 0) {
        radioLogs.forEach(log => {
            if (log.sentNr) {
                const nr = parseInt(log.sentNr);
                if (!isNaN(nr) && nr > maxNr) maxNr = nr;
            }
        });
    }
    const nextNr = maxNr + 1;
    sentNrInput.value = nextNr.toString().padStart(3, '0');
}

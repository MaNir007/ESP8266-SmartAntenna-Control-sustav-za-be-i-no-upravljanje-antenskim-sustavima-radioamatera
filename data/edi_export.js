// ---------------------------------------------------------------------------
// edi_export.js - Logika za generiranje EDI (REG1TEST) datoteka
// ---------------------------------------------------------------------------
// Obrađuje podatke iz forme, sprema osobne postavke u localStorage,
// formatira QSO zapise prema EDI standardu i pokreće preuzimanje.
// ---------------------------------------------------------------------------
'use strict';

let radioLogs = [];

document.addEventListener('DOMContentLoaded', () => {
    loadLogs();
    setupForm();
});

// ---------------------------------------------------------------------------
// Učitava logove iz sessionStorage-a (proslijeđeno s logger.html)
// ---------------------------------------------------------------------------
function loadLogs() {
    const raw = sessionStorage.getItem('currentLogs');
    if (raw) {
        radioLogs = JSON.parse(raw);
        document.getElementById('qsoCount').textContent = radioLogs.length;
        
        if (radioLogs.length > 0) {
            const dates = radioLogs.map(l => (l.time || '').slice(0, 10).replace(/-/g, '')).sort();
            document.getElementById('TDateStart').value = dates[0];
            document.getElementById('TDateEnd').value = dates[dates.length - 1];
            
            document.getElementById('PCall').value = radioLogs[0].my_call || ''; 
        }
    } else {
        showFormMessage('Nema logova za izvoz. Vratite se u Logger.', 'error');
    }
}

// ---------------------------------------------------------------------------
// Postavljanje event handlera i učitavanje spremljenih podataka
// ---------------------------------------------------------------------------
function setupForm() {
    const form = document.getElementById('ediForm');
    if (!form) return;
    
    form.addEventListener('submit', (e) => {
        e.preventDefault();
        generateEDI();
    });

    const savedInfo = localStorage.getItem('edi_personal_info');
    if (savedInfo) {
        const data = JSON.parse(savedInfo);
        Object.keys(data).forEach(key => {
            const el = document.getElementById(key);
            if (el) el.value = data[key];
        });
    }
}

// ---------------------------------------------------------------------------
// Glavna funkcija za generiranje EDI datoteke iz podataka s forme
// ---------------------------------------------------------------------------
function generateEDI() {
    const formData = new FormData(document.getElementById('ediForm'));
    const info = Object.fromEntries(formData.entries());

    const personalKeys = ['PCall', 'PWWLo', 'PExch', 'PAdr1', 'PAdr2', 'PClub', 
                          'RName', 'RCall', 'RHBBS', 'RPhon', 'RAdr1', 'RCity', 
                          'RPoCo', 'RCoun', 'STXEq', 'SRXEq', 'SPowe', 'SAnte', 
                          'SAntH_Gr', 'SAntH_Sea', 'MOpe'];
    const personalInfo = {};
    personalKeys.forEach(k => personalInfo[k] = info[k]);
    localStorage.setItem('edi_personal_info', JSON.stringify(personalInfo));

    const lines = [];
    lines.push(`[REG1TEST;1]`);
    lines.push(`TName=${info.TName}`);
    lines.push(`TDate=${info.TDateStart};${info.TDateEnd}`);
    lines.push(`PCall=${info.PCall}`);
    lines.push(`PWWLo=${info.PWWLo}`);
    lines.push(`PExch=${info.PExch || info.PWWLo}`);
    lines.push(`PAdr1=${info.PAdr1}`);
    lines.push(`PAdr2=${info.PAdr2}`);
    lines.push(`PSect=${info.PSect}`);
    lines.push(`PBand=${info.PBand}`);
    lines.push(`PClub=${info.PClub}`);
    lines.push(`RName=${info.RName}`);
    lines.push(`RCall=${info.RCall || info.PCall}`);
    lines.push(`RAdr1=${info.RAdr1}`);
    lines.push(`RAdr2=`); 
    lines.push(`RPoCo=${info.RPoCo}`);
    lines.push(`RCity=${info.RCity}`);
    lines.push(`RCoun=${info.RCoun}`);
    lines.push(`RPhon=${info.RPhon}`);
    lines.push(`RHBBS=${info.RHBBS}`);
    
    const operators = info.MOpe.split(/\s+/).filter(o => o);
    lines.push(`MOpe1=${operators[0] || info.PCall}`);
    lines.push(`MOpe2=${operators.slice(1).join(' ')}`);
    
    lines.push(`STXEq=${info.STXEq}`);
    lines.push(`SPowe=${info.SPowe}`);
    lines.push(`SRXEq=${info.SRXEq}`);
    lines.push(`SAnte=${info.SAnte}`);
    lines.push(`SAntH=${info.SAntH_Gr};${info.SAntH_Sea}`);
    
    lines.push(`CQSOs=${radioLogs.length};1`);
    lines.push(`CQSOP=0`);
    lines.push(`CWWLs=0;0;0`);
    lines.push(`CWWLB=0`);
    lines.push(`CExcs=0;0;0`);
    lines.push(`CExcB=0`);
    lines.push(`CDXCs=0;0;0`);
    lines.push(`CDXCB=0`);
    lines.push(`CToSc=0`);
    lines.push(`CODXC=;`);

    lines.push(`[Remarks]`);
    if (info.Remarks) lines.push(info.Remarks);

    lines.push(`[QSORecords;${radioLogs.length}]`);
    
    radioLogs.forEach((log, index) => {
        const dateRaw = (log.time || '').slice(0, 10).replace(/-/g, ''); 
        const date = dateRaw.length === 8 ? dateRaw.slice(2) : dateRaw; 
        
        const time = (log.time || '').slice(11, 16).replace(':', '');
        
        let mode = '1'; 
        const m = (log.modulation || '').toUpperCase();
        if (m === 'CW') mode = '2';
        else if (m === 'FM') mode = '6';
        else if (['RTTY', 'DIGITAL', 'FT8', 'FT4'].includes(m)) mode = '9';
        
        const rstS = log.rstSent || (m === 'CW' ? '599' : '59');
        const serS = (index + 1).toString().padStart(3, '0');
        
        let rstR = log.rstReceived || (m === 'CW' ? '599' : '59');
        let serR = '';
        
        const rstCleanup = rstR.replace(/\s+/g, '');
        if (rstCleanup.length >= 5) {
             rstR = rstCleanup.slice(0, 2);
             serR = rstCleanup.slice(2);
        } else if (rstR.includes(' ')) {
            const parts = rstR.split(' ');
            rstR = parts[0];
            serR = parts[1];
        }

        const qsoLine = `${date};${time};${(log.callsign || '').toUpperCase()};${mode};${rstS};${serS};${rstR};${serR};;${(log.qthlocator || '').toUpperCase()};1;;;;`;
        lines.push(qsoLine);
    });

    const content = lines.join('\r\n'); 
    const filename = `${info.PCall}_${info.PBand.replace(/\s/g, '')}.edi`;
    
    downloadFile(content, filename, 'text/plain');
    showFormMessage('EDI datoteka je uspješno generirana i preuzeta.', 'success');
}

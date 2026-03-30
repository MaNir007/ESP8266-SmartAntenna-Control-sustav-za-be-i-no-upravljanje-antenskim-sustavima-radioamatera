// ---------------------------------------------------------------------------
// common.js - Zajedničke funkcije i globalne varijable
// ---------------------------------------------------------------------------
// Sadrži temeljne funkcije za mrežnu komunikaciju (REST API),
// upravljanje UI elementima (status konekcije, obavijesti) i 
// pomoćne alate poput izračuna QTH lokatora.
// ---------------------------------------------------------------------------

const API_BASE_URL = `http://${window.location.hostname}`;

// ---------------------------------------------------------------------------
// Pomoćna funkcija za slanje HTTP zahtjeva
// ---------------------------------------------------------------------------
async function sendHttpRequest(endpoint, method = 'GET', data = null) {
    const path = endpoint.startsWith('/') ? endpoint : `/${endpoint}`;
    const url = `${API_BASE_URL}${path}`;

    const options = {
        method: method,
        headers: {
            'Content-Type': 'application/json',
        },
    };

    if (data) {
        options.body = JSON.stringify(data);
    }

    try {
        const response = await fetch(url, options);
        
        if (!response.ok) {
            const errorText = await response.text();
            throw new Error(`HTTP error! Status: ${response.status}, Poruka: ${errorText}`);
        }

        updateConnectionStatus('connected');

        const contentType = response.headers.get("content-type");
        if (contentType && contentType.includes("application/json")) {
            return await response.json();
        } else {
            return await response.text();
        }

    } catch (error) {
        console.error(`Greška kod ${method} ${url}:`, error);
        updateConnectionStatus('disconnected');
        
        if (method !== 'GET') {
            showFormMessage(`Greška komunikacije: ${error.message}`, 'error');
        }
        throw error;
    }
}

// ---------------------------------------------------------------------------
// Ažurira vizualni status veze u zaglavlju stranice
// ---------------------------------------------------------------------------
function updateConnectionStatus(status) {
    const statusElement = document.getElementById('connectionStatus');
    if (!statusElement) return;

    statusElement.classList.remove('status-connected', 'status-disconnected', 'status-connecting', 'status-warning', 'status-error');
    
    let icon = '';
    let text = '';

    switch (status) {
        case 'connected':
            statusElement.classList.add('status-connected');
            icon = '<i class="fas fa-check-circle"></i>';
            text = 'Povezano';
            break;
        case 'disconnected':
            statusElement.classList.add('status-disconnected');
            icon = '<i class="fas fa-exclamation-triangle"></i>';
            text = 'Prekinuta veza';
            break;
        case 'connecting':
            statusElement.classList.add('status-connecting');
            icon = '<i class="fas fa-circle-notch fa-spin"></i>';
            text = 'Povezivanje...';
            break;
        case 'warning':
            statusElement.classList.add('status-warning');
            icon = '<i class="fas fa-satellite"></i>';
            text = 'Tražim GPS...';
            break;
        case 'error':
            statusElement.classList.add('status-error');
            icon = '<i class="fas fa-times-circle"></i>';
            text = 'Senzor Greška';
            break;
    }
    statusElement.innerHTML = `${icon} <span>${text}</span>`;
}

// ---------------------------------------------------------------------------
// Prikazuje poruku (toast notification) korisniku
// ---------------------------------------------------------------------------
function showFormMessage(message, type, timeout = 3000) {
    const formMessage = document.getElementById('formMessage');
    if (formMessage) {
        formMessage.textContent = message;
        formMessage.className = `message-area show ${type}`;

        if (formMessage.timeoutId) {
            clearTimeout(formMessage.timeoutId);
        }

        if (timeout > 0) {
            formMessage.timeoutId = setTimeout(() => {
                formMessage.classList.remove('show');
            }, timeout);
        }
    }
}

// ---------------------------------------------------------------------------
// Preuzimanje datoteka (npr. ADIF logovi)
// ---------------------------------------------------------------------------
function downloadFile(content, filename, type = 'text/plain;charset=utf-8') {
    const blob = new Blob([content], { type: type });
    const a = document.createElement('a');
    a.href = URL.createObjectURL(blob);
    a.download = filename;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(a.href);
}

// ---------------------------------------------------------------------------
// Izračunava Maidenhead Locator (QTH) iz koordinata
// ---------------------------------------------------------------------------
function calculateQthLocator(latitude, longitude) {
    let lon = longitude + 180;
    let lat = latitude + 90;

    let locator = "";


    locator += String.fromCharCode(65 + Math.floor(lon / 20));
    locator += String.fromCharCode(65 + Math.floor(lat / 10));


    locator += Math.floor((lon % 20) / 2).toString();
    locator += Math.floor(lat % 10).toString();


    locator += String.fromCharCode(97 + Math.floor((lon % 2) * 12)).toLowerCase();
    locator += String.fromCharCode(97 + Math.floor((lat % 1) * 24)).toLowerCase();

    return locator;
}


window.sendHttpRequest = sendHttpRequest;
window.updateConnectionStatus = updateConnectionStatus;
window.showFormMessage = showFormMessage;
window.downloadFile = downloadFile;
window.calculateQthLocator = calculateQthLocator;
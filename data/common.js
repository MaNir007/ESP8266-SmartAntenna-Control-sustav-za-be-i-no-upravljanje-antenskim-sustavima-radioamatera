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
    let container = document.getElementById('toast-container');
    if (!container) {
        container = document.createElement('div');
        container.id = 'toast-container';
        container.className = 'toast-container';
        document.body.appendChild(container);
    }
    
    const toast = document.createElement('div');
    toast.className = `toast toast-${type}`;
    
    let icon = '<i class="fas fa-info-circle"></i>';
    if(type === 'success') icon = '<i class="fas fa-check-circle"></i>';
    if(type === 'error') icon = '<i class="fas fa-times-circle"></i>';
    if(type === 'warning') icon = '<i class="fas fa-exclamation-triangle"></i>';

    toast.innerHTML = `<div class="toast-icon">${icon}</div><div class="toast-message">${message}</div>`;
    
    container.appendChild(toast);
    
    // Provocira reflow za animaciju
    void toast.offsetWidth;
    toast.classList.add('show');
    
    if (timeout > 0) {
        setTimeout(() => {
            toast.classList.remove('show');
            setTimeout(() => {
                if (container.contains(toast)) {
                    container.removeChild(toast);
                }
            }, 300);
        }, timeout);
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

// ---------------------------------------------------------------------------
// Custom Modal Notifications (Glassmorphism & Animated)
// ---------------------------------------------------------------------------
function createModal(title, message, type = 'alert') {
    return new Promise((resolve) => {
        const overlay = document.createElement('div');
        overlay.className = 'custom-modal-overlay';
        
        const box = document.createElement('div');
        box.className = 'custom-modal-box';
        
        const heading = document.createElement('h3');
        heading.innerHTML = title;
        
        const text = document.createElement('p');
        text.innerHTML = message;
        
        const btnContainer = document.createElement('div');
        btnContainer.className = 'custom-modal-buttons';

        const closeBtn = document.createElement('button');
        closeBtn.className = 'button primary';
        closeBtn.innerHTML = type === 'confirm' ? '<i class="fas fa-check"></i> Potvrdi' : '<i class="fas fa-check"></i> OK';
        
        let cancelBtn = null;
        if (type === 'confirm') {
            cancelBtn = document.createElement('button');
            cancelBtn.className = 'button tertiary';
            cancelBtn.innerHTML = '<i class="fas fa-times"></i> Odustani';
        }

        const closeModal = (result) => {
            overlay.classList.add('closing');
            setTimeout(() => {
                if (document.body.contains(overlay)) {
                    document.body.removeChild(overlay);
                }
                resolve(result);
            }, 300); // Wait for the modalFadeOut and modalPopDown transition
        };

        closeBtn.onclick = () => closeModal(true);
        if (cancelBtn) {
            cancelBtn.onclick = () => closeModal(false);
            btnContainer.appendChild(closeBtn);
            btnContainer.appendChild(cancelBtn);
        } else {
            btnContainer.appendChild(closeBtn);
        }

        box.appendChild(heading);
        box.appendChild(text);
        box.appendChild(btnContainer);
        overlay.appendChild(box);
        document.body.appendChild(overlay);
        
        setTimeout(() => closeBtn.focus(), 100);
    });
}

window.customAlert = function(message, title = '<i class="fas fa-info-circle"></i> Obavijest') {
    return createModal(title, message, 'alert');
};

window.customConfirm = function(message, title = '<i class="fas fa-question-circle"></i> Potvrda') {
    return createModal(title, message, 'confirm');
};

// ---------------------------------------------------------------------------
// Dark Mode Toggle
// ---------------------------------------------------------------------------
document.addEventListener('DOMContentLoaded', () => {
    // 1. Dark Mode
    const savedTheme = localStorage.getItem('theme');
    if (savedTheme === 'dark') {
        document.body.classList.add('dark-mode');
    }

    const themeToggleBtn = document.createElement('button');
    themeToggleBtn.className = 'theme-toggle-btn';
    themeToggleBtn.innerHTML = document.body.classList.contains('dark-mode') ? '<i class="fas fa-sun"></i>' : '<i class="fas fa-moon"></i>';
    themeToggleBtn.onclick = () => {
        document.body.classList.toggle('dark-mode');
        const isDark = document.body.classList.contains('dark-mode');
        localStorage.setItem('theme', isDark ? 'dark' : 'light');
        themeToggleBtn.innerHTML = isDark ? '<i class="fas fa-sun"></i>' : '<i class="fas fa-moon"></i>';
    };
    document.body.appendChild(themeToggleBtn);
});
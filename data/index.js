// index.js - Skripte specifične za index.html stranicu
document.addEventListener('DOMContentLoaded', () => {
    // Inicijalizacija pri učitavanju
    onLoadIndex();
    
    // Pretpostavljamo da common.js definira updateConnectionStatus
    if (typeof updateConnectionStatus === 'function') {
        updateConnectionStatus('connected');
    }
});

/**
 * Inicijalizira UI elemente i postavlja periodično osvježavanje.
 */
async function onLoadIndex() {
    // Inicijalno dohvati sve podatke odmah
    await Promise.all([
        getSensorReadings(),
        getAntennaStatus(),
        getServoAngle()
    ]);

    // Intervalno osvježavanje (svakih 5 sekundi)
    setInterval(getSensorReadings, 5000);
    setInterval(getAntennaStatus, 5000);
    setInterval(getServoAngle, 5000);

    // Selektori elemenata
    const angleSlider = document.getElementById('angleSlider');
    const angleInput = document.getElementById('angleInput');

    // Povezivanje Slidera i Input polja
    if (angleSlider && angleInput) {
        angleSlider.addEventListener('input', function() {
            angleInput.value = this.value;
            updateAngleDisplay(this.value);
        });

        angleInput.addEventListener('input', function() {
            let val = parseInt(this.value);
            if (isNaN(val)) val = 0;
            if (val < 0) val = 0;
            if (val > 359) val = 359;
            this.value = val;
            angleSlider.value = val;
            updateAngleDisplay(val);
        });
    }
}

function updateAntennaUI(antennaType) {
    const led = document.getElementById('antennaLed');
    const statusText = document.getElementById('antennaStatus');

    if (!led || !statusText) return;

    // Ažuriraj tekstualni prikaz
    statusText.textContent = antennaType;

    // Resetiraj klase (ukloni boje)
    led.classList.remove('yagi-active', 'gp-active');

    // Dodaj boju ovisno o tipu
    if (antennaType === 'GP') {
        led.classList.add('gp-active'); // Postaje CRVENA
    } else if (antennaType === 'Yagi') {
        led.classList.add('yagi-active'); // Postaje ZELENA
    }
}

/**
 * Ažurira prikaz kuta na kompasu.
 */
function updateAngleDisplay(angle) {
    const compassDegree = document.getElementById('compassDegree');
    const compassArrow = document.getElementById('compassArrow');

    if (compassDegree && compassArrow) {
        compassDegree.innerHTML = angle + '°';
        compassArrow.style.transform = `rotate(${angle}deg)`;
    }
}

// --- API POZIVI (Ispravljene putanje na /api/...) ---

async function setAntenna(antennaType) {
    try {
        // ISPRAVLJENO: Putanja mora odgovarati web_server.cpp ruti
        await sendHttpRequest('/api/set_antenna', 'POST', { type: antennaType });
        if (typeof showFormMessage === 'function') showFormMessage(`Antena: ${antennaType}`, 'success');
        await getAntennaStatus();
    } catch (e) { console.error("Greška pri promjeni antene:", e); }
}

async function setAngle(angle) {
    try {
        const parsedAngle = parseInt(angle);
        if (isNaN(parsedAngle)) return;
        
        // ISPRAVLJENO: Putanja mora biti /api/set_angle
        await sendHttpRequest('/api/set_angle', 'POST', { angle: parsedAngle });
        await getServoAngle(); 
    } catch (e) { console.error("Greška pri slanju kuta:", e); }
}

function sendAngleFromInput() {
    const angleInput = document.getElementById('angleInput');
    if (angleInput) setAngle(angleInput.value);
}

async function getSensorReadings() {
    try {
        const data = await sendHttpRequest('/api/sensor_readings');
        if (document.getElementById('temperature')) document.getElementById('temperature').innerHTML = data.temperature.toFixed(1);
        if (document.getElementById('humidity')) document.getElementById('humidity').innerHTML = data.humidity.toFixed(1);
        if (document.getElementById('voltage')) document.getElementById('voltage').innerHTML = data.voltage.toFixed(2);
    } catch (e) {
        console.error("Senzori nedostupni");
    }
}

async function getAntennaStatus() {
    try {
        const data = await sendHttpRequest('/api/antenna_status');
        updateAntennaUI(data.status);
    } catch (e) { updateAntennaUI('N/A'); }
}

async function getServoAngle() {
    try {
        const data = await sendHttpRequest('/api/servo_angle');
        const angle = parseInt(data.angle);
        if (!isNaN(angle)) {
            updateAngleDisplay(angle);
            // Sinkroniziraj i slider/input s trenutnim stanjem stepera
            if(document.activeElement.id !== 'angleInput' && document.activeElement.id !== 'angleSlider') {
                document.getElementById('angleSlider').value = angle;
                document.getElementById('angleInput').value = angle;
            }
        }
    } catch (e) { console.error("Kut nedostupan"); }
}

async function calibrateServo() {
    if (confirm("Antena je usmjerena točno na SJEVER (0°)? Potvrdite za resetiranje brojača koraka.")) {
        try {
            await sendHttpRequest('/api/calibrate_servo', 'POST');
            if (typeof showFormMessage === 'function') showFormMessage('Pozicija 0° postavljena!', 'success');
            await getServoAngle();
        } catch (e) { console.error("Greška pri kalibraciji:", e); }
    }
}
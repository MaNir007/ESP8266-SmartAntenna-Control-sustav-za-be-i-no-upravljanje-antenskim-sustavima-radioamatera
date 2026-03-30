// ---------------------------------------------------------------------------
// index.js - Skripte specifične za glavnu upravljačku stranicu (index.html)
// ---------------------------------------------------------------------------
// Upravlja interakcijom kompasa, sinkronizacijom slidera i input polja kuta,
// periodičkim osvježavanjem senzora i kontrolom aktivne antene.
// ---------------------------------------------------------------------------
document.addEventListener('DOMContentLoaded', () => {
    onLoadIndex();
    
    if (typeof updateConnectionStatus === 'function') {
        updateConnectionStatus('connected');
    }
});

// ---------------------------------------------------------------------------
// Inicijalizira UI elemente i postavlja periodično osvježavanje
// ---------------------------------------------------------------------------
async function onLoadIndex() {
    await Promise.all([
        getSensorReadings(),
        getAntennaStatus(),
        getServoAngle()
    ]);

    setInterval(getSensorReadings, 5000);
    setInterval(getAntennaStatus, 5000);
    setInterval(getServoAngle, 5000);

    const angleSlider = document.getElementById('angleSlider');
    const angleInput = document.getElementById('angleInput');

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

    statusText.textContent = antennaType;

    led.classList.remove('yagi-active', 'gp-active');

    if (antennaType === 'GP') {
        led.classList.add('gp-active'); 
    } else if (antennaType === 'Yagi') {
        led.classList.add('yagi-active'); 
    }
}

// ---------------------------------------------------------------------------
// Ažurira vizualni prikaz kuta na kompasu i strelici
// ---------------------------------------------------------------------------
function updateAngleDisplay(angle) {
    const compassDegree = document.getElementById('compassDegree');
    const compassArrow = document.getElementById('compassArrow');

    if (compassDegree && compassArrow) {
        compassDegree.innerHTML = angle + '°';
        compassArrow.style.transform = `rotate(${angle}deg)`;
    }
}



async function setAntenna(antennaType) {
    try {
        await sendHttpRequest('/api/set_antenna', 'POST', { type: antennaType });
        if (typeof showFormMessage === 'function') showFormMessage(`Antena: ${antennaType}`, 'success');
        await getAntennaStatus();
    } catch (e) { console.error("Greška pri promjeni antene:", e); }
}

async function setAngle(angle) {
    try {
        const parsedAngle = parseInt(angle);
        if (isNaN(parsedAngle)) return;
        
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
            if(document.activeElement.id !== 'angleInput' && document.activeElement.id !== 'angleSlider') {
                document.getElementById('angleSlider').value = angle;
                document.getElementById('angleInput').value = angle;
            }
        }
    } catch (e) {
        console.error("Kut nedostupan");
    }
}

// ---------------------------------------------------------------------------
// Kalibracija servo (stepper) motora - postavlja trenutni položaj na 0°
// ---------------------------------------------------------------------------
async function calibrateServo() {
    if (confirm("Antena je usmjerena točno na SJEVER (0°)? Potvrdite za resetiranje brojača koraka.")) {
        try {
            await sendHttpRequest('/api/calibrate_servo', 'POST');
            if (typeof showFormMessage === 'function') showFormMessage('Pozicija 0° postavljena!', 'success');
            await getServoAngle();
        } catch (e) { console.error("Greška pri kalibraciji:", e); }
    }
}
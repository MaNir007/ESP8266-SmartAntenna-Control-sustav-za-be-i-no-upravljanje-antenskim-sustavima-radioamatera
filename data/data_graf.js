const ctx = document.getElementById('sensorChart').getContext('2d');
const sensorChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Temperatura (°C)',
            borderColor: '#ff5252',
            backgroundColor: 'rgba(255, 82, 82, 0.1)',
            data: [],
            borderWidth: 2,
            yAxisID: 'y',
            tension: 0.3 
        }, {
            label: 'Vlažnost (%)',
            borderColor: '#2100b5',
            backgroundColor: 'rgba(33, 0, 181, 0.1)',
            data: [],
            borderWidth: 2,
            yAxisID: 'y1',
            tension: 0.3
        }]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
            y: { type: 'linear', display: true, position: 'left', grid: { color: 'rgba(255,255,255,0.1)' }, ticks: { color: '#000' } },
            y1: { type: 'linear', display: true, position: 'right', grid: { drawOnChartArea: false }, ticks: { color: '#000' } }
        },
        plugins: {
            legend: { labels: { color: 'black', font: { size: 14 } } }
        }
    }
});

// --- NOVO: Funkcija za učitavanje povijesti iz pozadine ---
async function loadHistory() {
    try {
        const response = await fetch('/api/history');
        const text = await response.text();
        
        if (!text) return; // Ako je datoteka prazna

        const lines = text.trim().split('\n');
        
        // Uzimamo zadnjih 50 zapisa iz datoteke da ne zagušimo graf
        const lastLines = lines.slice(-50); 

        lastLines.forEach((line, index) => {
            const parts = line.split(',');
            if (parts.length === 2) {
                // Budući da u CSV-u nemamo točno vrijeme (osim ako ga ne šalješ),
                // stavit ćemo oznaku "P" (Povijest) ili ostaviti prazno
                sensorChart.data.labels.push("H" + index); 
                sensorChart.data.datasets[0].data.push(parseFloat(parts[0]));
                sensorChart.data.datasets[1].data.push(parseFloat(parts[1]));
            }
        });
        
        sensorChart.update();
        console.log("Povijest uspješno učitana.");
    } catch (error) {
        console.error("Greška pri učitavanju povijesti:", error);
    }
}

async function updateData() {
    try {
        const response = await fetch('/data'); 
        if (!response.ok) throw new Error('Mrežni odgovor nije OK');
        
        const data = await response.json();
        const now = new Date().toLocaleTimeString();

        if (document.getElementById('temp-val')) document.getElementById('temp-val').innerText = data.temp.toFixed(1) + " °C";
        if (document.getElementById('hum-val')) document.getElementById('hum-val').innerText = data.hum.toFixed(1) + " %";
        
        const statusEl = document.getElementById('connectionStatus');
        if (statusEl) {
            statusEl.innerHTML = '<i class="fas fa-check-circle"></i> Online';
            statusEl.style.color = "#2ecc71";
        }

        // Drži maksimalno 50 točaka na ekranu (uključujući povijest)
        if (sensorChart.data.labels.length > 50) {
            sensorChart.data.labels.shift();
            sensorChart.data.datasets[0].data.shift();
            sensorChart.data.datasets[1].data.shift();
        }

        sensorChart.data.labels.push(now);
        sensorChart.data.datasets[0].data.push(data.temp);
        sensorChart.data.datasets[1].data.push(data.hum);
        sensorChart.update('none'); 

    } catch (error) {
        console.error("Greška pri dohvatu:", error);
        const statusEl = document.getElementById('connectionStatus');
        if (statusEl) {
            statusEl.innerHTML = '<i class="fas fa-exclamation-circle"></i> Offline';
            statusEl.style.color = "#ff5252";
        }
    }
}

// POKRETANJE
loadHistory().then(() => {
    // Tek nakon što se učita povijest, kreni s ažuriranjem uživo
    setInterval(updateData, 5000);
    updateData();
});
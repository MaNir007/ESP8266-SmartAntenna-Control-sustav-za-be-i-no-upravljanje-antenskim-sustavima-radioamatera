// Inicijalizacija grafa
const ctx = document.getElementById('sensorChart').getContext('2d');
const sensorChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: [], // Vremenske oznake
        datasets: [{
            label: 'Temperatura (°C)',
            borderColor: '#ff5252',
            backgroundColor: 'rgba(255, 82, 82, 0.1)',
            data: [],
            borderWidth: 2,
            yAxisID: 'y'
        }, {
            label: 'Vlaga (%)',
            borderColor: '#2100b5ff',
            backgroundColor: 'rgba(0, 173, 181, 0.1)',
            data: [],
            borderWidth: 2,
            yAxisID: 'y1'
        }]
    },
    options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
            y: { type: 'linear', display: true, position: 'left', grid: { color: '#333' } },
            y1: { type: 'linear', display: true, position: 'right', grid: { drawOnChartArea: false } }
        },
        plugins: {
            legend: { labels: { color: '#fff' } }
        }
    }
});

// Funkcija za dohvat podataka
async function updateData() {
    try {
        const response = await fetch('/data'); // ESP rute
        const data = await response.json();
        const now = new Date().toLocaleTimeString();

        // Ažuriraj tekstualne vrijednosti
        document.getElementById('temp-val').innerText = data.temp.toFixed(1) + " °C";
        document.getElementById('hum-val').innerText = data.hum.toFixed(1) + " %";
        document.getElementById('status').innerHTML = 'Status: <span class="online">Online</span>';

        // Dodaj podatke u graf (limit na 20 točaka)
        if (sensorChart.data.labels.length > 20) {
            sensorChart.data.labels.shift();
            sensorChart.data.datasets[0].data.shift();
            sensorChart.data.datasets[1].data.shift();
        }

        sensorChart.data.labels.push(now);
        sensorChart.data.datasets[0].data.push(data.temp);
        sensorChart.data.datasets[1].data.push(data.hum);
        sensorChart.update();

    } catch (error) {
        console.error("Greška pri dohvatu:", error);
        document.getElementById('status').innerHTML = 'Status: <span style="color:red">Greška u vezi</span>';
    }
}

// Osvježavaj svakih 5 sekundi
setInterval(updateData, 5000);
updateData(); // Prvi poziv odmah
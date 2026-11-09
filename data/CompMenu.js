var xmlhttp = new XMLHttpRequest();
var GliderKeeperConfig = {};

window.onload = initialSetup;

function initialSetup() {

    document.getElementById('btn-save').addEventListener('click', setCompsValues);
    getCompsValues();

}

function getCompsValues() {

    document.getElementById('idURLCompetition').value = window.localStorage.getItem('idURLCompetition');
    document.getElementById('idCompetition').value = window.localStorage.getItem('idCompetition');
    document.getElementById('idPilotNum').value = window.localStorage.getItem('idPilotNum');
}

function setCompsValues() {
    if (document.getElementById('idForm').checkValidity())
    {
        window.localStorage.setItem('idURLCompetition', document.getElementById('idURLCompetition').value);
        window.localStorage.setItem('idCompetition', document.getElementById('idCompetition').value);
        window.localStorage.setItem('idPilotNum', document.getElementById('idPilotNum').value);
        document.getElementById('idMsg').style.display="block";
        document.getElementById('button-line').style.display="none";
    }
}

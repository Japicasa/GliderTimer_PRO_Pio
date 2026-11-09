var xmlhttp = new XMLHttpRequest();
var GliderKeeperConfig = {};

window.onload = initialSetup;

function initialSetup() {

    // Ahora leemos los datos de la Wifi
    xmlhttp.onreadystatechange = fillGliderKeeperConfig;
    xmlhttp.open("GET", "GliderKeeperConfig.txt", true);
    xmlhttp.setRequestHeader('Cache-Control', 'no-cache');
    setTimeout(function () { xmlhttp.send(); }, 1000);
}

function fillGliderKeeperConfig() {
    if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
        var str = this.responseText;
        var datos = str.split('\n');
        for (var i = 0; i < datos.length; i++) {
            if (datos[i].trim().length > 0) {
                var conf = datos[i].split(':');
                GliderKeeperConfig[conf[0].trim()] = conf[1].trim();
            }
        };
        if (GliderKeeperConfig.GliderCompEnabled.toUpperCase() == 'Y')
        {
            document.getElementById("compbtn1").style.display="block";
            document.getElementById("compbtn2").style.display="block";
        }
    }
}



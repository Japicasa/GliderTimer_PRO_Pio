let pilotsPromise;
let pilotsResultsPromise;
let roundsResultsPromise;
let fullResutsPromise;
let GliderKeeperConfig = {};
let settings = {};
let xreq = new XMLHttpRequest();
let xmlhttpui = new XMLHttpRequest();
let devURL = "./";
let endload = false;
let uiVersion;

window.onload = function () {
  //document.getElementById("id_select_lnk").addEventListener("click", handleSelectComp);
  document.getElementById("id_showresult_lnk").addEventListener("click", handleShowResults);
  document.getElementById("id_system_lnk").addEventListener("click", handleSystem);
  document.getElementById("id_viewPilots_lnk").addEventListener("click", handleViewPilots);
  document.getElementById("id_gliderScore_lnk").addEventListener("click", handleGliderCore);
  document.getElementById("id_WifiSetup_btn").addEventListener("click", handleWifiSetup);
  document.getElementById("id_Settings_btn").addEventListener("click", handleSettings);
  document.getElementById("id_UpdateFirmware").addEventListener("click", handleUpdateFirmware);
  document.getElementById("id_UpdateUserInterface_btn").addEventListener("click", handleUpdateUI);
  document.getElementById("id_btn_send_GS_files").addEventListener("click", handleSendGSFiles);
  document.getElementById("id_btn_gen_sched").addEventListener("click", handleGenerateScheduleFile);
  // getUiVersion(); 
  getFwVersion(); // Get Fw version data
 
  showWait(false);
};

function getUiVersion() {
  xmlhttpui.onreadystatechange = setUiVersion;
  xmlhttpui.open("GET", devURL + "version.txt", true);
  xmlhttpui.setRequestHeader("Cache-Control", "no-cache");
  xmlhttpui.send();
}

function getFwVersion() {
  xmlhttpui.onreadystatechange = setFwVersion;
  xmlhttpui.open("GET", devURL + "get_fw_version", true);
  xmlhttpui.setRequestHeader("Cache-Control", "no-cache");
  xmlhttpui.send();
}


function setUiVersion() {
  if (this.readyState == 4)
    if (this.status == 200) {
      localStorage.setItem("uiVersion", xmlhttpui.responseText);
      //showVersion();
    } else {
      showWait(false);
      showMsg("Cannot get version data");
    }
}

function setFwVersion() {
  if (this.readyState == 4)
    if (this.status == 200) {
      localStorage.setItem("fwVersion", xmlhttpui.responseText);
      showVersion();
    } else {
      showWait(false);
      showMsg("Cannot get version data");
    }
}

function showVersion() {
  uiVersion = localStorage.getItem("uiVersion");
  fwVersion = localStorage.getItem("fwVersion");

  // uiVersion = uiVersion == null ? "-.--" : uiVersion;
  // fwVersion = fwVersion == null ? "-.--" : fwVersion.slice(-4);
  fwVersion = fwVersion == null ? "-.--" : fwVersion;

  document.getElementById("idVersion").innerHTML =
  "<strong>GliderTimer</strong> <h6><b><small>Fw: " + fwVersion+ "</b></small></h6>";
  // "<strong>GliderTimer</strong> <h6><small>Fw: <b>" + fwVersion + "</b>"+ " Ui: <b>" + uiVersion + "</b>"+ "";
}

function handleSelectComp() {
  hideDivs();
  document.getElementById("id_select_comp").style.display = "block";
  document.getElementById("id_select_competition_btn").addEventListener("click", setCompetition);
}

function setCompetition() {
  showWait(true);
  let comp = document.getElementById("id_select_competition").value;
  fetch("/setcompetition?comp=" + comp)
    .then((resp) => {
      if (!resp.ok) throw Error(resp.status + " " + resp.statusText);
      showWait(false);
      showMsg("Competition set", "alert-success");
    })
    .catch((error) => {
      showWait(false);
      showMsg(error, "alert-warning");
    });
}

function handleShowResults() {
  let pilots = [];
  let fresults = [];
  let presults = [];
  let rresults = [];

  hideDivs();
  document.getElementById("id_results_area").style.display = "block";

  pilotsPromise = getObjectFromCSV("pilotos.csv").then((res) => {
    pilots = res;
  });

  fullResutsPromise = getObjectFromCSV("fresults.csv").then((res) => {
    fresults = res;
  });

  roundsResultsPromise = getObjectFromCSV("rresults.csv").then((res) => {
    rresults = res;
  });

  pilotsResultsPromise = getObjectFromCSV("presults.csv").then((res) => {
    presults = res;
  });

  const promises = [pilotsPromise, pilotsResultsPromise, roundsResultsPromise, fullResutsPromise];

  Promise.allSettled(promises).then((results) => {
    showFinalResults(pilots, fresults);
    showResultsbyRound(pilots, rresults);
    showResultsbyPilot(pilots, presults);
  });
}

function handleSystem() {
  hideDivs();
  document.getElementById("id_system_area").style.display = "block";
}

function handleViewPilots() {
  hideDivs();
  pilotsPromise = getObjectFromCSV("pilotos.csv").then((res) => {
    showPilots(res);
  });
}

function handleGliderCore() {
  hideDivs();
  document.getElementById("id_GS_files").style.display = "block";
}

function handleWifiSetup() {
  hideDivs();
  showWait(true);

  // Ahora leemos los datos de la Wifi
  const headers = new Headers();
  headers.append("pragma", "no-cache");
  headers.append("cache-control", "no-cache");

  pilotsPromise = fetch("./GliderKeeperConfig.txt", { method: "GET", headers: headers })
    .then((res) => {
      return res.text();
    })
    .then((config) => {
      showWifiSetup(config);
    })
    .catch((error) => {
      showWait(false);
      showMsg(error, "alert-warning");
    });
}

function handleSettings() {
  hideDivs();
  showWait(true);

  // Ahora leemos los datos de la Wifi
  const headers = new Headers();
  headers.append("pragma", "no-cache");
  headers.append("cache-control", "no-cache");

  fetch("./settings.txt", { method: "GET", headers: headers })
    .then((res) => {
      return res.text();
    })
    .then((config) => {
      showSettings(config);
      showValues();
    })
    .catch((error) => {
      showWait(false);
      showMsg(error, "alert-warning");
    });
}

function handleUpdateFirmware() {
  hideDivs();
  showWait(true);

  showUpdateFirmwareSection();
}

function handleUpdateUI() {
  hideDivs();
  showWait(true);

  showUpdateUISection();
}

function handleSendGSFiles() {
  let formData = new FormData();

  if (document.getElementById("idPilotMasterFile").value) {
    formData.append("update", document.getElementById("idPilotMasterFile").files[0], "pilot_master_file.csv");

    xreq.onloadstart = function (e) {
      document.getElementById(`idGSWait`).style.display = "block";
      document.getElementById(`upload_GS_files`).style.display = "none";
    };

    xreq.onload = pilotosEnviados;
    xreq.open("POST", devURL + "uploader");
    xreq.send(formData);
  } else {
    document.getElementById(`idGSWait`).style.display = "none";
    document.getElementById(`idFileError`).innerHTML = "Pilot Master File";
    document.getElementById(`idGSError`).style.display = "block";
    document.getElementById(`upload_GS_files`).style.display = "block";
  }
}

function pilotosEnviados(xrq) {
  let formData = new FormData();

  if (this.status != 200) {
    document.getElementById(`idGSWait`).style.display = "none";
    document.getElementById(`idGSError`).style.display = "block";
  } else {
    if (document.getElementById("idDrawTable").value) {
      formData.append("update", document.getElementById("idDrawTable").files[0], "draw_table.csv");
      xreq.onload = drawEnviado;
      xreq.open("POST", devURL + "uploader");
      xreq.send(formData);
    } else {
      document.getElementById(`idGSWait`).style.display = "none";
      document.getElementById(`idFileError`).innerHTML = "Missing Draw File";
      document.getElementById(`idGSError`).style.display = "block";
      document.getElementById(`upload_GS_files`).style.display = "block";
    }
  }
}

function drawEnviado(xrq) {
  let formData = new FormData();

  if (this.status != 200) {
    document.getElementById(`idGSWait`).style.display = "none";
    document.getElementById(`idGSError`).style.display = "block";
  } else {
    if (document.getElementById("idRoundsSchedule").value) {
      formData.append("update", document.getElementById("idRoundsSchedule").files[0], "scheduled_rounds.csv");
      xreq.onload = scheduleEnviado;
      xreq.open("POST", devURL + "uploader");
      xreq.send(formData);
    } else {
      document.getElementById(`idGSWait`).style.display = "none";
      document.getElementById(`idOKText`).innerHTML = "Files Uploaded OK";
      document.getElementById(`idGSOK`).style.display = "block";

      document.getElementById(`idFileError`).innerHTML = "Warning: Schedule File Missing";
      document.getElementById(`idGSError`).style.display = "block";
      document.getElementById(`upload_GS_files`).style.display = "block";
    }
  }
}

function scheduleEnviado(xrq) {
  let formData = new FormData();

  if (this.status != 200) {
    document.getElementById(`idGSWait`).style.display = "none";
    document.getElementById(`idGSError`).style.display = "block";
  } else {
    // muestra el resultado
    document.getElementById(`idGSWait`).style.display = "none";
    document.getElementById(`idOKText`).innerHTML = "Files Uploaded OK";
    document.getElementById(`idGSOK`).style.display = "block";
    document.getElementById(`idGSError`).style.display = "none";
    document.getElementById(`upload_GS_files`).style.display = "block";
  }
}

function handleGenerateScheduleFile() {
  document.getElementById("id_btn_gen_sched").href = "/sched?secs=" + document.getElementById("idSecs").value;
}

function showPilots(pilots) {
  document.getElementById("id_save_pilots_btn").addEventListener("click", savePilots);
  document.getElementById("id_cancel_pilots_btn").addEventListener("click", () => {
    location.href = "/";
  });

  document.getElementById("id_pilots_area").style.display = "block";
  let bodyPilots = document.getElementById("id_body_pilots");
  bodyPilots.innerHTML = "";

  for (let numpilot = 0; numpilot < pilots.length; numpilot++) {
    const pilot = pilots[numpilot];
    let row = document.createElement("tr");
    // row.setAttribute("class", "row");

    let pilot_idEle = document.createElement("td");
    let serialnumberEle = document.createElement("td");
    let nameEle = document.createElement("td");
    let nick_nameEle = document.createElement("td");

    // pilot_idEle.setAttribute("class", "col-1");
    // serialnumberEle.setAttribute("class", "col-3");
    // nick_nameEle.setAttribute("class", "col-3");
    // nameEle.setAttribute("class", "col-5");

    nick_nameEle.setAttribute("contenteditable", "true");
    nameEle.setAttribute("contenteditable", "true");

    // nick_nameEle.setAttribute("style", "border: 1px solid black");
    // nameEle.setAttribute("style", "border: 1px solid black");
    pilot_idEle.setAttribute("style", "background: #dee2e6; ");
    serialnumberEle.setAttribute("style", "background: #dee2e6;");

    pilot_idEle.innerHTML = pilot.pilot_id;
    serialnumberEle.innerHTML = pilot.serialnumber;
    nick_nameEle.innerHTML = pilot.nick_name;
    nameEle.innerHTML = pilot.name;

    row.appendChild(pilot_idEle);
    row.appendChild(serialnumberEle);
    row.appendChild(nameEle);
    row.appendChild(nick_nameEle);

    bodyPilots.appendChild(row);
  }
}

function savePilots() {
  const table = document.getElementById("id_pilots_tbl");
  let datos = "pilot_id;serialnumber;name;nick_name;\n";
  for (let r = 1; r < table.rows.length; r++) {
    if (r > 1) {
      datos += "\n";
    }
    for (let c = 0, m = table.rows[r].cells.length; c < m; c++) {
      datos += table.rows[r].cells[c].innerHTML + ";";
    }
  }

  fetch("/savePilots", {
    method: "POST",
    body: datos,
  })
    .then((success) => {
      showWait(false);
      showMsg("Pilots list saved", "alert-success");
    })
    .catch((error) => {
      showWait(false);
      showMsg(error, "alert-warning");
    });
}

function showSettings(config) {
  settings.volume = 6;
  settings.brightness = 250;
  settings.collector = 0;
  settings.groups = 0;
  settings.scheduled = 0;
  settings.panel_type = 0;
  settings.baud_rate = 9600;
  settings.comp_step = 0;
  settings.background_volume = 10;
  settings.announcement_volume = 20;
  settings.strict_timing = 0;

  config = config.replace(/\r/g, "");
  const datos = config.split("\n");
  for (var i = 0; i < datos.length; i++) {
    if (datos[i].trim().length > 0) {
      var conf = datos[i].split("=");
      settings[conf[0].trim()] = conf[1].trim();
    }
  }
  document.getElementById("idVolume").value = settings.volume;
  document.getElementById("idBrightness").value = settings.brightness;
  document.getElementById("idCollector").checked = settings.collector == "1";
  document.getElementById("idPanelType").value = settings.panel_type;
  document.getElementById("idBaud").value = settings.baud_rate;
  document.getElementById("idBackgroundVolume").value = settings.background_volume;
  document.getElementById("idAnnouncementVolume").value = settings.announcement_volume;
  if (settings.groups == 0 && settings.scheduled == 0) {
    document.getElementById("idCompType").value = 1;
  }

  if (settings.groups == 0 && settings.scheduled == 1) {
    document.getElementById("idCompType").value = 2;
  }

  if (settings.groups == 1) {
    document.getElementById("idCompType").value = 3;
  }
  document.getElementById("idStrictLandTiming").checked = settings.strict_timing == "1";

  showWait(false);
  document.getElementById("id_Settings").style.display = "block";
  // // change form action
  // document.getElementById("update_wifi_frm").action = "./saveWifi";
}

function showWifiSetup(config) {
  config = config.replace(/\r/g, "");
  const datos = config.split("\n");
  for (var i = 0; i < datos.length; i++) {
    if (datos[i].trim().length > 0) {
      var conf = datos[i].split(":");
      GliderKeeperConfig[conf[0].trim()] = conf[1].trim();
    }
  }
  document.getElementById("idSSID").value = GliderKeeperConfig.WiFiSSID;
  document.getElementById("idWiFiPass").value = GliderKeeperConfig.WifiPass;
  document.getElementById("idSSID1").value = GliderKeeperConfig.WiFiSSID1;
  document.getElementById("idWiFiPass1").value = GliderKeeperConfig.WifiPass1;

  showWait(false);
  document.getElementById("id_Wifi_Setup").style.display = "block";
  // change form action
  document.getElementById("update_wifi_frm").action = "./saveWifi";
}

function showUpdateFirmwareSection() {
  showWait(false);
  document.getElementById("id_Update_Firmware").style.display = "block";
  $("#update-firmware-btn-save").prop("disabled", true);
  $(".custom-file-input").on("change", function () {
    var fileName = $(this).val().split("\\").pop();
    if (fileName.match(/(Timer|GT)(_Fw_(STD|mini|PRO))_[0-9]?[0-9].[0-9][0-9]\.bin/gm)) {
      $(this).siblings(".custom-file-label").removeClass("text-danger").addClass("text-success").html(fileName);
      $("#update-firmware-btn-save").prop("disabled", false);
    } else {
      $(this).siblings(".custom-file-label").addClass("text-danger").removeClass("text-success").html("Filename Error");
      $("#update-firmware-btn-save").prop("disabled", true);
    }
  });
  document.getElementById("update-firmware-btn-save").addEventListener("click", () => sendFile("FW"));
}

function showUpdateUISection() {
  showWait(false);
  document.getElementById("id_Update_UI").style.display = "block";
  $("#update-ui-btn-save").prop("disabled", true);
  $(".custom-file-input").on("change", function () {
    var fileName = $(this).val().split("\\").pop();
    if (fileName.match(/(Timer|GT)(_UI_(mini|PRO))_[0-9]?[0-9].[0-9][0-9]\.bin/gm)) {
      $(this).siblings(".custom-file-label").removeClass("text-danger").addClass("text-success").html(fileName);
      $("#update-ui-btn-save").prop("disabled", false);
    } else {
      $(this).siblings(".custom-file-label").addClass("text-danger").removeClass("text-success").html("Filename Error");
      $("#update-ui-btn-save").prop("disabled", true);
    }
  });
  document.getElementById("update-ui-btn-save").addEventListener("click", () => sendFile("UI"));
}

function sendFile(fileType) {
  let formData = new FormData();
  endload = false;

  formData.append("update", document.getElementById(`id${fileType}File`).files[0]);

  xreq.onloadstart = function (e) {
    document.getElementById(`id${fileType}Wait`).style.display = "block";
    document.getElementById(`id${fileType}Form`).style.display = "none";
  };
  xreq.onloadend = function (e) {
    console.log("end");
    if (!endload) {
      document.getElementById(`id${fileType}Wait`).style.display = "none";
      document.getElementById(`id${fileType}OK`).style.display = "block";
      enload = true;
    }
  };

  xreq.addEventListener("error", function (e) {
    console.log("error");
    if (!endload) {
      if (document.getElementById(`id${fileType}OK`).style.display != "block") {
        document.getElementById(`id${fileType}Wait`).style.display = "none";
        document.getElementById(`id${fileType}Error`).style.display = "block";
      }
      endload = true;
    }
  });

  xreq.addEventListener("abort", function (e) {
    console.log("abort");
    if (!endload) {
      if (document.getElementById(`id${fileType}OK`).style.display != "block") {
        document.getElementById(`id${fileType}Wait`).style.display = "none";
        document.getElementById(`id${fileType}Error`).style.display = "block";
      }
      endload = true;
    }
  });

  xreq.upload.addEventListener(
    "progress",
    function (evt) {
      if (evt.lengthComputable) {
        console.log("loaded: " + evt.loaded + " / total: " + evt.total);
        document.getElementById(`id${fileType}Progress`).style.width = (evt.loaded / evt.total) * 100 + "%";
        if (evt.loaded == evt.total) {
          document.getElementById(`id${fileType}Wait`).style.display = "none";
          document.getElementById(`id${fileType}OK`).style.display = "block";
          xreq.abort();
        }
      }
    },
    false
  );
  xreq.open("POST", devURL + "update");
  xreq.send(formData);
}

/**
 * This functiosn shows final resulta table
 * @param {*} pilots
 * @param {*} fresults
 */

function showFinalResults(pilots, fresults) {
  let tabletbl = document.getElementById("id_result_table");
  tabletbl.innerHTML = "";
  for (const result of fresults) {
    if (result.Rank == undefined) break;
    let tr = document.createElement("tr");

    // rank
    let td = document.createElement("td");
    td.innerHTML = result.Rank;
    tr.appendChild(td);

    // Id
    td = document.createElement("td");
    td.innerHTML = result.id;
    tr.appendChild(td);

    // pilot
    td = document.createElement("td");
    td.innerHTML = findPilot(result.id, pilots);
    tr.appendChild(td);

    // fai
    td = document.createElement("td");
    td.innerHTML = result.TotFAI;
    tr.appendChild(td);

    // round 1
    td = document.createElement("td");
    td.innerHTML = result.Round1;
    tr.appendChild(td);

    // round 2
    td = document.createElement("td");
    td.innerHTML = result.Round2;
    tr.appendChild(td);

    // round 3
    td = document.createElement("td");
    td.innerHTML = result.Round3;
    tr.appendChild(td);

    // round 4
    td = document.createElement("td");
    td.innerHTML = result.Round4;
    tr.appendChild(td);

    // round 5
    td = document.createElement("td");
    td.innerHTML = result.Round5;
    tr.appendChild(td);

    // round 6
    td = document.createElement("td");
    td.innerHTML = result.Round6;
    tr.appendChild(td);

    // tPenalty
    td = document.createElement("td");
    td.innerHTML = result.tPenalty;
    tr.appendChild(td);

    // Discarded
    td = document.createElement("td");
    td.innerHTML = result.Discarded;
    tr.appendChild(td);

    tabletbl.appendChild(tr);
  }
}

/**
 * This functiosn shows results by round
 * @param {*} pilots
 * @param {*} rresults
 */
function showResultsbyRound(pilots, rresults) {
  let tabletbl = document.getElementById("id_round_result_table");
  tabletbl.innerHTML = "";
  for (const result of rresults) {
    if (result.Round == undefined) break;
    let tr = document.createElement("tr");

    // round
    let td = document.createElement("td");
    td.innerHTML = result.Round;
    tr.appendChild(td);

    // Id
    td = document.createElement("td");
    td.innerHTML = result.id;
    tr.appendChild(td);

    // pilot
    td = document.createElement("td");
    td.innerHTML = findPilot(result.id, pilots);
    tr.appendChild(td);

    // F5Jh
    td = document.createElement("td");
    td.innerHTML = result.F5Jh;
    tr.appendChild(td);

    // time
    td = document.createElement("td");
    const minutes = Math.floor(result.time / 100);
    const seconds = result.time - minutes * 100;
    td.innerHTML = minutes + ":" + seconds;
    tr.appendChild(td);

    // landing
    td = document.createElement("td");
    td.innerHTML = result.ldist;
    tr.appendChild(td);

    // raw_score
    td = document.createElement("td");
    td.innerHTML = result.raw_score;
    tr.appendChild(td);

    // FAI_pts
    td = document.createElement("td");
    td.innerHTML = result.FAI_pts;
    tr.appendChild(td);

    // penalty
    td = document.createElement("td");
    td.innerHTML = result.penalty;
    tr.appendChild(td);

    // Round_rank
    td = document.createElement("td");
    td.innerHTML = result.Round_rank;
    tr.appendChild(td);

    tabletbl.appendChild(tr);
  }
}

/**
 * This functiosn shows results by pilot
 * @param {*} pilots
 * @param {*} rresults
 */
function showResultsbyPilot(pilots, presults) {
  let tabletbl = document.getElementById("id_pilots_result_table");
  tabletbl.innerHTML = "";
  for (const result of presults) {
    if (result.id == undefined) break;
    let tr = document.createElement("tr");

    // Id
    let td = document.createElement("td");
    td.innerHTML = result.id;
    tr.appendChild(td);

    // pilot
    td = document.createElement("td");
    td.innerHTML = findPilot(result.id, pilots);
    tr.appendChild(td);

    // round
    td = document.createElement("td");
    td.innerHTML = result.Round;
    tr.appendChild(td);

    // F5Jh
    td = document.createElement("td");
    td.innerHTML = result.F5Jh;
    tr.appendChild(td);

    // time
    td = document.createElement("td");
    const minutes = Math.floor(result.time / 60);
    const seconds = result.time - minutes * 60;
    td.innerHTML = minutes + ":" + seconds;
    tr.appendChild(td);

    // landing
    td = document.createElement("td");
    td.innerHTML = result.ldist;
    tr.appendChild(td);

    // raw_score
    td = document.createElement("td");
    td.innerHTML = result.raw_score;
    tr.appendChild(td);

    // FAI_pts
    td = document.createElement("td");
    td.innerHTML = result.FAI_pts;
    tr.appendChild(td);

    // penalty
    td = document.createElement("td");
    td.innerHTML = result.penalty;
    tr.appendChild(td);

    // Round_rank
    td = document.createElement("td");
    td.innerHTML = result.Round_rank;
    tr.appendChild(td);

    tabletbl.appendChild(tr);
  }
}

/***************************************************
 * A partir de aquí funciones auxiliares           *
 ***************************************************/

/**
 * @param {*} id
 * @param {*} pilots
 * @returns String with pilot name
 */
function findPilot(id, pilots) {
  let pilot = pilots.find((p) => p.serialnumber == id);

  let pilotname = "Pilot " + id;
  if (pilot) {
    pilotname = pilot.name;
  }
  return pilotname;
}

/**
 * Hides all pages. Call this before enable a section
 */
function hideDivs() {
  const appSreensDivs = document.getElementsByClassName("appScreens");

  for (let div of appSreensDivs) {
    div.style.display = "none";
  }
}

/**
 * This function returns a Promise from a call to a csv file in the server
 *
 * @param {string} file
 * @returns {Object}
 */

function getObjectFromCSV(file) {
  let resultado = [];

  return fetch(file)
    .then((response) => {
      return response.text().then((filecsv) => {
        resultado = csv2json(filecsv);
        return resultado;
      });
    })
    .catch((error) => {
      console.error(error);
      return resultado;
    });
}

/**
 * Convert a csv string to a json object
 * @param  {String} str
 * @param  {String=";"} delimiter
 * @returns {Object}
 */
const csv2json = (str, delimiter = ";") => {
  // const rows = str.slice(str.indexOf('\n') + 1).split('\n');
  const rows = str.split("\n");
  const titles = rows.shift().split(delimiter);

  return rows.map((row) => {
    const values = row.split(delimiter);
    return titles.reduce((object, curr, i) => ((object[curr] = values[i]), object), {});
  });
};

/**
 * Muestra un mensaje de aviso arriba
 * @param {string} msg
 * @param {string} alertType
 */
function showMsg(msg, alertType) {
  const divmsg = document.getElementById("id_alert");

  divmsg.classList.remove("alert-warning");
  divmsg.classList.remove("alert-success");
  divmsg.classList.add(alertType);

  document.getElementById("id_warnig_msg").innerHTML = msg;
  divmsg.style.display = "block";
}

/**
 * Show spinner
 * @param {boolean} show
 */

function showWait(show) {
  const spinner = document.getElementById("id_spinner");
  spinner.style.display = show ? "block" : "none";
}

function showValues() {
  let volume = document.getElementById("idVolume").value;
  let brightness = document.getElementById("idBrightness").value;
  let step = document.getElementById("idStep").value;
  let baud = document.getElementById("idBaud").value;
  let announcementVolume = document.getElementById("idAnnouncementVolume").value;
  let backgroundVolume = document.getElementById("idBackgroundVolume").value;

  document.getElementById("idLabelVolume").innerHTML = `Beep Volume <strong>${volume}</strong>`;
  document.getElementById("idLabelBrightness").innerHTML = `Brightness <strong>${brightness}</strong>`;
  // document.getElementById("idLabelStep").innerHTML = `Competition Steps <strong>${step}</strong>`;
  document.getElementById("idLabelBaud").innerHTML = `Baud Rate <strong>${baud}</strong>`;
  document.getElementById(
    "idLabelAnnouncementVolume"
  ).innerHTML = `Announcement Volume <strong>${announcementVolume}</strong>`;
  document.getElementById(
    "idLabelBackgroundVolume"
  ).innerHTML = `Background Volume <strong>${backgroundVolume}</strong>`;
}


function sendSettings() {
  document.getElementById("idWaitSavingSettings").style.display = "block";
  document.getElementById("id_Settings").style.display = "none";
  const idCompType = document.getElementById("idCompType").value;

  let settings = "volume=" + document.getElementById("idVolume").value + "\n";
  settings += "brightness=" + document.getElementById("idBrightness").value + "\n";
  settings += "collector=" + (document.getElementById("idCollector").checked ? "1" : "0") + "\n";
  switch (idCompType) {
    case "1":
      settings += "groups=0\n";
      settings += "scheduled=0\n";
      break;
    case "2":
      settings += "groups=0\n";
      settings += "scheduled=1\n";
      break;
    case "3":
      settings += "groups=1\n";
      settings += "scheduled=1\n";

      break;
    default:
      settings += "groups=0\n";
      settings += "scheduled=0\n";
      break;
  }
  settings += "panel_type=" + document.getElementById("idPanelType").value + "\n";
  settings += "baud_rate=" + document.getElementById("idBaud").value + "\n";
  settings += "comp_step=0\n";
  settings += "background_volume=" + document.getElementById("idBackgroundVolume").value + "\n";
  settings += "announcement_volume=" + document.getElementById("idAnnouncementVolume").value + "\n";
  settings += "strict_timing=" + (document.getElementById("idStrictLandTiming").checked ? "1" : "0") + "\n";

  let formData = new FormData();
  let settingsBlob = new Blob([settings], { type: "text/plain" });
  formData.append("settings.txt", settingsBlob, "settings.txt");
  fetch("/uploader", {
    // Your POST endpoint
    method: "POST",
    body: formData, // This is your file object
  })
    .then((response) => {
      if (response.status == 200) {
        document.getElementById("idWaitSavingSettings").style.display = "none";
        document.getElementById("idSaveSettingsOK").style.display = "block";
      } else {
        document.getElementById("idWaitSavingSettings").style.display = "none";
        document.getElementById("idSaveSettingsError").style.display = "block";
      }
    })
    .catch((response) => {
      document.getElementById("idWaitSavingSettings").style.display = "none";
      document.getElementById("idSaveSettingsError").style.display = "block";
    });
}

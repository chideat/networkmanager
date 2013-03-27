function panelPopup(position, flag) {
    if(flag) {
        if(position === "left" && $("#panel-left").is(":hidden")) {
            $("#panel-left").show().focus();
        }
        else if(position === "right" && $("#panel-right").is(":hidden")) {
            $("#panel-right").show().focus();
        }
    }
    else {
        $(".panel").hide();
    }
}

function appMore() {
    var canvas = document.getElementById("more");
    var context = canvas.getContext("2d");
    context.fillStyle = "#5A5A5A";
    context.fillRect(18, 0, 12, 48);
    context.fillRect(0, 18, 48, 12);
}

/*
  app {
       name: media-player,
       url: icon path,
       action: action
  }
  */
function trayIcon(app) {
    $("#tray-icon").prepend('<div class="icon" data-link="' + app.name + '"><img src="' + app.url + '"></div>');
}

$(document).ready(function(){
    appMore();
    Q_Desktop.panelPopup.connect(panelPopup);
    
    $(document).on("click", ".app", function(){
        Q_Desktop.run($(this).attr("cmd"));
    })
    .on("click", ".icon", function(){
        if($(this).attr("data-link") === "networkmanager") {
            alert("networkmanager");
        }
        else if($(this).attr("data-link") === "media-player") {
            alert("media-player");
        }
        else if($(this).attr("data-link") === "battery") {
            alert("battery");
        }
        else if($(this).attr("data-link") === "bluetooth") {
            alert("bluetooth");
        }
        else if($(this).attr("data-link") === "volume") {
            alert("volume");
        }
        else if($(this).attr("data-link") === "shutdown") {
            alert("shutdown");
        }
    });
    
    
});
document.onmousemove=function(event) {
    var point = {x:event.clientX, y:event.clientY};
    if(point.y <= document.height/4 || event.y >= 3 * document.height/4) {
        if(point.x <= 1) {
            $("#panel-left").show();
        }
        else if(point.x >= document.width - 1) {
            $("#panel-right").show();
        }
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
    
    $(document).on("click", ".app", function(){
        Operator.run($(this).attr("cmd"));
    })
    .on("click", ".tray-icon ", function(){
        if($("#" + $(this).attr("data-link")).is(":hidden")) {
            if($(this).attr("data-link") === "networkmanager") {
                $(".container").width(300).show().find("#networkmanager").show();
            }
            else if($(this).attr("data-link") === "media-player") {
            }
            else if($(this).attr("data-link") === "battery") {
            }
            else if($(this).attr("data-link") === "bluetooth") {
            }
            else if($(this).attr("data-link") === "volume") {
            }
            else if($(this).attr("data-link") === "shutdown") {
            } 
        }
        else
            $(".container").hide().find(".box").hide();
    })
    .on("mouseleave", ".panel", function(){
        $(".panel").hide().find(".container").hide().find(".box").hide();
    })
    .on("click", ".showpwd", function() {
        if($(this).is(":checked")) {
            $(this).parent().find(".textbox").attr("type", "text");
        }
        else {
            $(this).parent().find(".textbox").attr("type", "password");
        }
    });
});
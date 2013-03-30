//functions define
/*  arg_1 id,
  * arg_2 uuid
  * arg_3 type
  * arg_4 wired: auto or not | wireless: encrypt or not 
  * arg_5 strength
  */
function js_insertItem(arg_1, arg_2, arg_3, arg_4, arg_5) {
    var isWireless = (arg_3==="802-11-wireless"?true : false);
    var item = '<div class="item" id="' + arg_2 +'">' +
            '<div><div class="icon">'+
            '<img src="qrc:/img/'+ ( isWireless ?"wireless":"wired") + '.png" alt="wired"></img>'+
            '</div><div class="id">'+ arg_1 +'</div>' +
            (isWireless ? (arg_4 ? '<div class="strength"><img src="img/signal-100-secure.png"></div>':'<div class="strength"><img src="img/signal-100.png"></div>'):'') + 
            '<div class="status" style="display:none;"><span>Connected</span></div></div>' + 
            '<div class="setting">' +
            '<div class="connect button" uuid=\"' + arg_2 + '\">'+
            '<div class="off"><span>关</span></div> <div class="vr"></div>'+ 
            '<div class="on"><span>开</span></div></div>' + 
            '</div></div>'; 
    if(isWireless) {
        $(".wireless").find(".items").append($(item));
    }
    else {
        $(".wired").find(".items").append($(item));
    }
}

function remove(uuid) {
    $("#" + uuid).remove();
}
/** init UI
  *  network is enabled or not
  * wireless is enabled or not
  */
function load() {
    if(Network.networkUp) {
        $(".wired").show().parent().next().hide();
        if(!Network.wirelessHardwareUp || !Network.wirelessUp) {
            $("#flying_mode").addClass("enabled").find(".on").css('background-color', 'black').parent()
            .find(".off").css('background-color', 'blue');
            $(".wireless").hide();
        }
    }
    else {
        $("#connections").hide();
    }
}

function updateProperty(uuid, key, value) {
    //get refered uuid node
    $item = $("#" + uuid);
    if(key === 'Ssid') {
        $item.find(".id").text(value);
    }
    else if(key === 'Strength') {
        $item.find(".cover").width($item.find('.strength').width() - 
                                   $item.find(".strength").width() * str / 100);
    }
}

function button(target) {
    if(target.hasClass("enabled")) {
        //off
        target.removeClass("enabled").find(".off").css('background-color', 'black').parent()
        .find(".on").css('background-color', 'blue');
        return 'off';
    }
    else {
        target.addClass("enabled").find(".on").css('background-color', 'black').parent()
        .find(".off").css('background-color', 'blue');
        return 'on';
    }
}

function speed(upload, download) {
    if(upload > 1024) {
        upload = upload / 1024;
        upload = upload + " Mb/s";
    }
    else {
        upload = upload + " Kb/s";
    }
    if(download > 1024) {
        download = download / 1024;
        download = download + " Mb/s";
    }
    else {
        download = download + " Kb/s";
    }
    
    $(".download").find("span").text(download);
    $(".upload").find("span").text(upload);
}

function detect(result) {
    $(".detect-result").find(".one").hide().next().text(result).show();
}

function disConnect(index) {
    if(index === 1) {
        $(".wired").find(".button").find(".off").css('background-color', 'black').parent()
        .find(".on").css('background-color', 'blue').removeClass("enabled");
        $(".wired").find(".flag").hide();
    }
    else  if(index === 2) {
        $(".wireless").find(".button").find(".off").css('background-color', 'black').parent()
        .find(".on").css('background-color', 'blue').removeClass("enabled");
        $(".wireless").find(".flag").hide();
    }
}

function needPassword(uuid) {
    $(".password").show().attr("uuid", uuid).find("textbox").attr({"value": "", "type": "password"})
    .next().attr("checked", false);
}

//actions
$(document).ready(function(){
    $(document).on("click",".item", function() {
        /* if($(this).find(".setting").is(':hidden'))
            $(this).find(".setting").show();
        else {
            $(this).find(".setting").hide();
        }*/
        $(this).find(".setting").show();
    })
    .on("mouseover", ".item", function() {
        $(this).css({background: "#BEBEBE"});
    })
    .on("mouseleave", ".item",function() {
        $(this).find(".setting").hide();
        $(this).css({background: "#9F9F9F"});
    })
    .on("click", ".connect", function() {
        if(button($(this)) === 'on') {
            $(this).parent().parent().find(".status").show();
            Network.tryConnect($(this).attr("uuid"));
        }
        else {
            $(this).parent().parent().find(".status").hide();
            Network.disConnect($(this).attr("uuid"));
        }
    })
    .on("click", ".submit", function() {
        $(".password").hide();
        Network.tryConnect($(".password").attr("uuid"), 
                           $(".password").find(".textbox").val());
    });
    
    $("#flying_mode").on("click", function(){
        if(button($(this)) === 'off') {
            Network.enableWireless(true);
            $(".wireless").show();
        }
        else {
            Network.enableWireless(false);
            $(".wireless").hide();
        }
    });
    
    $("#network-forbid").on("click", function() {
        if(button($(this)) === 'off') {
            Network.enableNetwork(true);
        }
        else {
            Network.enableNetwork(false);
            disConnect(1);
            disConnect(2);
        }
    });
    
    $("#popup").on("click", function() {
        $("#advanced").show();
    });
    
    $("#advanced").on("mouseleave", function() {
        $(this).hide();
    });
    
    $("#popdown").on("click", function(event) {
        $(this).parent().hide();
        event.stopPropagation();
    });
    
    $(".action").on("mouseover", function () {
        $(this).css('background-color', '#BEBEBE');
    })
    .on("mouseleave", function () {
        $(this).css('background-color', '#8E8E8E');
    });
    
    $("#connection-new").on("click", function() {
        Operator.nmEditor();
    });
    
    $("#network-detect").on("click", function(){
        Network.networkCheck();
        $(this).find(".detect-result").show();
    })
    .on("mouseleave", function(){
        $(this).find(".detect-result").hide();
    });
    
    
    
    //here connect javascript function to qt signals
    Network.loadFinished.connect(load);
    Operator.removeItem.connect(remove);
    Network.accessPointProperty.connect(updateProperty);
    Operator.speed.connect(speed);
    Network.checkfinished.connect(detect);
    Network.closeConn.connect(disConnect);
    Network.needPassword.connect(needPassword);
});
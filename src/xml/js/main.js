//functions define
function js_insertItem(id, uuid, type, auto) {
    item = '<div class="item" uuid="' + uuid +'">' +
            '<div>' + 
            '<div class="icon"><img src="qrc:/img/'+ (type==="802-11-wireless"?"wireless":"wired") +'.png" alt="wired"></img></div> ' + 
            '<div class="id">'+ id +'</div><div class="status"></div>' + 
            (type === '802-11-wireless' ? '<div class="strength"><div class="cover"></div></div>':'') +
             '</div>' + 
             '<div class="setting">' +
             '<div class="connect button" uuid=\"' + uuid + '\"><div class="off"><span>关</span></div> <div class="vr"></div><div class="on"><span>开</span></div><div class="switch"></div></div>' + 
             '</div></div>'; 
    if(type==='802-11-wireless') {
        $(".wireless").find(".items").append($(item));
    }
    else {  
        $(".wired").find(".items").append($(item));
    }
}

function load() {
    if(Q_Network.networkUp) {
        //allow showing device list
        $("#device-list").show();
        $("#network").addClass("enabled");
        
        if(Q_Network.wirelessUp) {
            $("#wireless").addClass("enabled");
        }
        else {
            $("#wireless").removeClass("enabled");
        }
    }
    else {
        $("#network").removeClass("enabled");
    }
}

function strength(uuid, str) {
    //get refered uuid node
    $node = $("#" + uuid).find(".cover");
    $node.width($node.parent().width() - $node.parent().width() * str / 70);
}


function button(target) {
    if(target.hasClass("enabled")) {
        //off
        target.removeClass("enabled").find(".switch").css('margin-left', '0');
        return 'off';
    }
    else {
        target.addClass("enabled").find(".switch").css('margin-left', '30px');
        return 'on';
    }
}

//actions
$(document).ready(function(){
    $(document).on("click",".item", function() {
        if($(this).find(".setting").is(':hidden'))
            $(this).find(".setting").show();
        else {
            $(this).find(".setting").hide();
        }
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
            Q_Network.tryConnect($(this).attr("uuid"));
        }
        else {
            Q_Network.tryConnect($(this).attr("uuid"));
        }
    });
    
    
    $("#flying_mode").on("click", function(){
        if(button($(this)) === 'on') {
            Q_Network.enableWireless(true);
        }
        else {
            Q_Network.enableWireless(false);
        }
    });
    
    $("#network-forbid").on("click", function() {
        if(button($(this)) === 'off') {
            Q_Network.enableNetwork(true);
        }
        else {
            Q_Network.enableNetwork(false);
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
        Q_Operator.nmEditor();
    });
    
    //here connect javascript function to qt signals
    Q_Network.loadFinished.connect(load);
});

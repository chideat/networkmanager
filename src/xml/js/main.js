function enableDevice(device) {
}

$(document).ready(function(){
    $(".item").on("click",function() {
        $(this).find(".setting").show();
    });


    $(".item").on("mouseleave",function() {
        $(this).find(".setting").hide();
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
});

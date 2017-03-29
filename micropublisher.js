
  /*
(function($){
  Drupal.behaviors.micropublisherPrev = {
    attach: function(context) {
      var total = $('#micropublisher-embedded-img-total').val();
      var active = $('#micropublisher-embedded-img-active').val();
      var next = Number(active) - 1;
      if(next < 1){
        next = total;
      }
      $('#micropublisher-embedded-img-active').val(next);
      // change counter
      $('.micropublisher_embedded_totalimg').html("Total images " + total + "/" + next);
      //images
      $('img[id^=micropublisher-embedded-img]').each(function() {
        //        var url = $(this).attr('src');
        if($(this).attr('rel') != next){
          $(this).hide();
        }
        else{
          $(this).show();
          var src = $(this).attr('uri');
          $(".micropublisher_img_hidden", context).val(src);
        }
      });
      //        alert('oki');
      return false;
    }
  }
  // next button
  Drupal.behaviors.micropublisherNext = {
    attach: function(context) {
      var total = $('#micropublisher-embedded-img-total').val();
      var active = $('#micropublisher-embedded-img-active').val();
      var next = 1 + Number(active);
      if(next > total){
        next = 1;
      }
      $('#micropublisher-embedded-img-active').val(next);
      // change counter
      $('.micropublisher_embedded_totalimg').html("Total images " + total + "/" + next);
      //images
      $('img[id^=micropublisher-embedded-img]').each(function() {
        //        var url = $(this).attr('src');
        if($(this).attr('rel') != next){
          $(this).hide();
        }
        else{
          $(this).show();
          var src = $(this).attr('uri');
          $(".micropublisher_img_hidden").val(src);
        }
      });
      //        alert('oki');
      return false;
    }
  };
  
  
  // next button
  Drupal.behaviors.micropublisherImages = {
    attach: function(context) {
      //images
      $('img[id^=micropublisher-embedded-img]').each(function() {
        var total = $('#micropublisher-embedded-img-total').val();
        var active = $('#micropublisher-embedded-img-active').val();
        // hide all without active one
        var url = $(this).attr('src');
        if($(this).attr('rel') != active){
          $(this).hide();
        }
        // show counter
        $('.micropublisher_embedded_totalimg').html("Total images " + total + "/" + active);
      });
    }
  };
  
    // play video
  Drupal.behaviors.micropublisherPlayVideo = function(sid) {
    $('#' + sid + '_img').empty();
    $('#' + sid + '_info').empty();
    $('#' + sid + '_code').show();
  };
  
  // next button
  Drupal.behaviors.micropublisherBind = {
    attach: function(context) {
      // Bind some funciton for
      //    console.log(Drupal.settings.micropublisher.activated_providers);
      //url.match(/:\/\/(www\.)?(.[^/:]+)/)[2];
      // validate the url on paste
      $('#edit-field-micropublisher-und-0-upload', context).bind({
        'keyup paste': function(e){
          //var activated_providers = Drupal.settings.micropublisher.activated_providers;
          //var providers = Drupal.settings.micropublisher.providers;
          var ob = $(this);
          if (e.type == 'paste'){
            setTimeout(function(){
              var url = ob.val();
              var domain = url.match(/:\/\/(www\.)?(.[^/:]+).com/)[2];
              if(providers[domain]){
                if(activated_providers[domain]){
                  //                alert('in');
                  $('.form-item-field-micropublisher-und-0 .description', context).html('Add a link to the photo that you\'d like to share. ');
                }
                else {
                  //                alert('not in');
                  $('#edit-field-micropublisher-und-0-upload', context).val(null);
                  $('#edit-field-micropublisher-und-0-upload', context).append('Invalid  URL');
                  $('.form-item-field-micropublisher-und-0 .description', context).html('<div class="messages error">http://www.' +domain+'.com content are not compatible.</div>');
                }
              }
            }, 1);
          }
        }
      });
    }
  }
})(jQuery);


  */
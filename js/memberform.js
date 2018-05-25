(function($, Drupal) {
  Drupal.ajax.prototype.commands.memberform_ga = function(ajax, response, status) {
    if(window.ga && ga.create) {
      ga('set', { page: response.path, title: response.title });
      ga('send', 'pageview');
    }
  }
})(jQuery, Drupal);

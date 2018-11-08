function copyToClipboard(text, el) {
  var copyTest = document.queryCommandSupported('copy');
  var elOriginalText = el.attr('data-original-title');

  if (copyTest === true) {
    var copyTextArea = document.createElement("textarea");
    copyTextArea.value = text;
    document.body.appendChild(copyTextArea);
    copyTextArea.select();
    try {
      var successful = document.execCommand('copy');
      var msg = successful ? 'Copied!' : 'Whoops, not copied!';
      el.attr('data-original-title', msg).tooltip('show');
    } catch (err) {
      console.log('Oops, unable to copy');
    }
    document.body.removeChild(copyTextArea);
    el.attr('data-original-title', elOriginalText);
  } else {
    // Fallback if browser doesn't support .execCommand('copy')
    window.prompt("Copy to clipboard: Ctrl+C or Command+C, Enter", text);
  }
}

$(document).ready(function() {
  $('.js-tooltip').tooltip();

  $('.js-copy').click(function() {
    var targ = $(this).attr('data-copy');
    var text = document.getElementById(targ).innerHTML 
    var el = $(this);
    copyToClipboard(text, el);
  });


  var objDivs = document.getElementsByClassName('svg_zoom_pan');
  for (let objDiv of objDivs) {
    objDiv.addEventListener('load', function() {
      var svgElem = objDiv.contentDocument.getElementsByTagName('svg')[0];
      svgElem.removeAttribute('height');
      svgElem.removeAttribute('width');
//    svgElem.setAttribute('width', "100%");
//    svgElem.setAttribute('max-width', "100%!important");
//    svgElem.setAttribute('height', "600px");
      svgElem.setAttribute('viewBox', "0 0 1500 600");
//    svgElem.setAttribute('preserveAspectRatio', "xMinYMin meet");
      svgPanZoom(svgElem, {
        panEnabled: true,
        fit: true,
        controlIconsEnabled: true,
        zoomEnabled: true,
        dblClickZoomEnabled: true,
        mouseWheelZoomEnabled: true,
        zoomScaleSensitivity: 0.2,
        minZoom: 0.1,
        maxZoom: 1000
      });
    });
  }
});

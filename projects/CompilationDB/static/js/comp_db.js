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

  var observer = new MutationObserver(function(mutL,obs) {
    for(var mutation of mutL) {
      if (mutation.target.style.display != 'none') {
        var svgElem = mutation.target.getElementsByTagName('svg')[0];
        svgPanZoom(svgElem, {
          zoomEnabled: true,
          controlIconsEnabled: true
        });
      }
    }
  });

  var svgDivs = document.getElementsByClassName('compilation_unit_graphviz_body');
  for (let svgDiv of svgDivs) {
    observer.observe(svgDiv, { attributes: true });
  }
});

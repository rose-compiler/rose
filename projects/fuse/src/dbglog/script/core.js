  function unhide(blockID) {
    var parentDiv = document.getElementById("div"+blockID);
    if (parentDiv) {
      // Hide the parent div
      parentDiv.className=(parentDiv.className=='hidden')?'unhidden':'hidden';
      // Get all the tables
      var childTbls = document.getElementsByTagName("table");
      condition = new RegExp("table"+blockID+"[0-9_-]*");
      for (var i=0; i<childTbls.length; i++){ 
        var child = childTbls[i];
        // Set the visibility status of each child table to be the same as its parent div
        if ("table"+blockID!=child.id && child.nodeType==1 && child.id!=undefined && child.id.match(condition)) {
            child.className=parentDiv.className;
        }
      }
    }
  }

  function FileHash() {
    this.items = new HashTable();
    this.leaf  = new HashTable();
    this.objectType = "FileHash";
  }
  
  var fileInfo = new FileHash();
  var loadedAnchors = {};
  
  function recordFile(fileID, rKey, rVal) {
      if(fileID.length==0) return;
      //console.debug("recordFile("+fileID+", #"+fileID.length+", rKey="+rKey+", rVal="+rVal+"): "+fileInfoStr());
      fileInfo = recordFile_rec(fileInfo, fileID, 0, rKey, rVal);
      console.debug("recordFile="+fileInfoStr());
  }
  
  function recordFile_rec(subfileInfo, fileID, index, rKey, rVal) {
    //console.debug("recordFile_rec("+fileID+", #"+fileID.length+", rKey="+rKey+", rVal="+rVal+"), typeof subfileInfo="+(typeof subfileInfo)+": "+fileInfoStr());
    if(typeof subfileInfo === 'undefined') subfileInfo = new FileHash();
    
    var fileKey = fileID[index];
    //console.debug("fileKey="+fileKey);
    if(index == fileID.length-1) {
      // If the given fileKey has not yet been mapped within this fileInfo sub-tree, create a HashTable for it
      if(!subfileInfo.leaf.hasItem(fileKey)) subfileInfo.leaf.setItem(fileKey, new HashTable());
      subfileInfo.leaf.getItem(fileKey).setItem(rKey, rVal);
    } else {
      subfileInfo.items.setItem(fileKey, recordFile_rec(subfileInfo.items.getItem(fileKey), fileID, index+1, rKey, rVal));
    }
    return subfileInfo;
  }
  
  function getFile(fileID, rKey) {
    console.debug("getFile: fileID="+fileID+", rKey="+rKey);
    console.debug(fileInfoStr());
      if(fileID.length==0) return undefined;
      return getFile_rec(fileInfo, fileID, 0, rKey);
  }
  
  function getFile_rec(subfileInfo, fileID, index, rKey) {
    var fileKey = fileID[index];
    if(index == fileID.length-1) {
      return subfileInfo.leaf.getItem(fileKey).getItem(rKey);
    } else {
      return getFile_rec(subfileInfo.items.getItem(fileKey), fileID, index+1, rKey);
    }
  }
  
  function fileInfoStr() {
    return fileInfoStr_rec(fileInfo, "");
  }
  
  function fileInfoStr_rec(subfileInfo, indent) {
    var str = "{\n";
    subfileInfo.leaf.each(function(k, v) {
      str += indent + "    L: " + k + " => \n";
      str += indent + "        "+v.str(indent + "        ")+"\n";
    });
    subfileInfo.items.each(function(k, v) {
      str += indent + "    I: " + k + " => ";
      if(v.objectType == "FileHash")
        str += fileInfoStr_rec(v, indent+"    ");
      else
        str += v;
      str += "\n";
    });
    return str + "\n" + indent + "}";
  }

  var scriptEltID=0;
  function loadURLIntoDiv(doc, url, divName, continuationFunc) {
    var xhr= new XMLHttpRequest();
    xhr.open('GET', url, true);
    xhr.onreadystatechange= function() {
      //Wait until the data is fully loaded
      if (this.readyState!==4) return;
      // Option 1:
      //doc.getElementById(divName).innerHTML= this.responseText;
      // Option 2:
      var scriptNode = document.createElement('script_'+scriptEltID);
      scriptEltID++;
      scriptNode.innerHTML = this.responseText;
      doc.getElementById(divName).appendChild(scriptNode);
            
      if(typeof continuationFunc !== 'undefined')
        continuationFunc();
    };
    xhr.send();
  }
    
  // From http://www.javascriptkit.com/javatutors/loadjavascriptcss.shtml
  //  and http://stackoverflow.com/questions/950087/how-to-include-a-javascript-file-in-another-javascript-file
  function loadjscssfile(filename, filetype, continuationFunc){
    if (filetype=="css"){ //if filename is an external CSS file
      var fileref=document.createElement("link")
      fileref.setAttribute("rel", "stylesheet")
      fileref.setAttribute("type", "text/css")
      fileref.setAttribute("href", filename)
    //if filename is a external script file
    } else { 
      var fileref=document.createElement('script')
      fileref.setAttribute("type", filetype)
      fileref.setAttribute("src", filename)
    }
    
    if(typeof continuationFunc !== 'undefined') {
      fileref.onreadystatechange = continuationFunc;
      fileref.onload = continuationFunc;
    }
    
    if (typeof fileref!="undefined")
      document.getElementsByTagName("head")[0].appendChild(fileref)
  }
  
  // Loads the given file and calls continuationFunc() on its contents
  function loadFile(url, continuationFunc) {
    var xhr= new XMLHttpRequest();
    xhr.open('GET', url, true);
    xhr.onreadystatechange= function() {
      //Wait until the data is fully loaded
      if (this.readyState!==4) return;
      continuationFunc(this.responseText);
    };
    xhr.send();
  }
  
  // Loads the given file. The file is assumed to contain the paths of scripts, one per line.
  // After the loading is finished, calls continuationFunc()
  function loadScriptsInFile(doc, url, continuationFunc) {
    loadFile(url, function(text) {
        var lines=text.split("\n");
        
        function recursiveLoad(lines, i) {
          if(i<lines.length && lines[i] != "") {
            var fields = lines[i].split(" ");
            loadjscssfile(fields[0], fields[1],
              function() {
                recursiveLoad(lines, i+1);
              }
            )
          } else 
            continuationFunc();
        }
        recursiveLoad(lines, 0);
      });
  }
  
  function loadSubFile(detailDoc, detailURL, detailDivName, sumDoc, sumURL, sumDivName, scriptURL, continuationFunc) {
    loadURLIntoDiv(detailDoc, detailURL, detailDivName,
                   function() { 
                     loadURLIntoDiv(sumDoc, sumURL, sumDivName,
                                    function() { 
                                      loadjscssfile(scriptURL, "text/javascript", continuationFunc);
                                    } ); } );
    
    
  }
  
  function loadAnchorScriptsFile(anchorFileID, continuationFunc) {
    if(!(anchorFileID in loadedAnchors))
      return loadjscssfile('script/anchor_script.'+anchorFileID, 'text/javascript', 
                function() { loadedAnchors[anchorFileID]=1; return continuationFunc(); } );
    else
      return continuationFunc();
  }

  function highlightLink(blockID, newcolor) {
    var sumLink = top.summary.document.getElementById("link"+blockID);
    if(sumLink) sumLink.style.backgroundColor=newcolor;
  }
  function focusLinkSummary(blockID, e) {
    if(typeof e !== 'undefined') {
      e = e || window.event;
      if('cancelBubble' in e) {
        e.cancelBubble = true;
        top.summary.location = "summary.0.html#anchor"+blockID;
      }
    } else {
      top.summary.location = "summary.0.html#anchor"+blockID;
    }
  }
  
	function focusLinkDetail(blockID) {
		top.detail.location = "detail.0.html#anchor"+blockID;
	}

  
  // Anchors
  var anchors = new HashTable();
  
  function anchor(fileID, blockID) {
    this.fileID  = fileID;
    this.blockID = blockID;
    console.debug("anchor("+fileID+", "+blockID+")");
  }
  
  // Opens the given file ID (if needed) and shifts the browser's focus to it.
  // The file ID is encoded in the pair prefix/suffix. Initially prefix is empty and suffix contains the entire
  // fileID. In each iteration we shift one element from suffix to prefix, open the file at that ID and repeat.
  // The function modifies the arguments, so they cannot be used again after this call.
  // When the anchor opens all the required files, it called continuationFunc, if it is provided
  function goToAnchor(prefix, suffix, continuationFunc) {
    console.debug("goToAnchor("+prefix+", "+suffix+")");
    // If the suffix is empty, we're done
    if(suffix.length == 0) {
      if(typeof continuationFunc !== 'undefined')
        return continuationFunc();
      else 
        return undefined;
    }
    
    // Move an entry from suffix to prefix
    prefix.push(suffix.splice(0, 1)[0]);
    
    // If this fileID has not yet been loaded
    if(!getFile(prefix, "loaded")) {
      console.debug('Loading '+prefix); 
      var nextFunc = getFile(prefix, 'loadFunc');
      getFile(prefix, 'loadFunc')(
        function() { goToAnchor(prefix, suffix, continuationFunc); }
      );
    // Otherwise, if it has already been loaded, load its child file within the file ID
    } else {
      goToAnchor(prefix, suffix, continuationFunc);
    }
    return undefined;
  }
  
/*  var next = function() { 
    var next = function() { 
      var next = undefined;
      console.debug('Loaded [1, 2, 2, 1]='+getFile([1, 2, 2, 1], 'loaded')); 
      if(!getFile([1, 2, 2, 1], 'loaded')) { 
        console.debug('Loading [1, 2, 2, 1]'); 
        getFile([1, 2, 2, 1], 'loadFunc')(next); } 
      else if(typeof next !== 'undefined') 
      { next(); }
      undefined;
    };
    console.debug('Loaded [1, 2, 2]='+getFile([1, 2, 2], 'loaded')); 
    if(!getFile([1, 2, 2], 'loaded')) { 
      console.debug('Loading [1, 2, 2]'); 
      getFile([1, 2, 2], 'loadFunc')(next);
    } else if(typeof next !== 'undefined') { next(); } 
    undefined;
  };
  console.debug('Loaded [1, 2]='+getFile([1, 2], 'loaded')); 
  if(!getFile([1, 2], 'loaded')) { 
    console.debug('Loading [1, 2]'); 
    getFile([1, 2], 'loadFunc')(next);
  } 
  else if(typeof next !== 'undefined') { next(); } 
  undefined;*/
  
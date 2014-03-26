

$(document).ready( regalweb );

function regalweb() {
  $("body").animate( { backgroundColor: "#444444" }, 200, function() {
    $.get("/debug/begin", function() {
      createMainTab();
    } );
  } );
}

function imageLoaded( img ) {
  var source = $("#" + img + "source")[0];
  var canvas = $("#" + img + "canvas")[0];
  var parent = source.parentNode;

  var gl = canvas.gl || ( canvas.gl = canvas.getContext('webgl') || canvas.getContext("experimental-webgl") );

  var rd = canvas.renderData || ( canvas.renderData = new Object() );

  if( ! rd.tex ) {
    rd.tex = gl.createTexture();
  }

  gl.bindTexture(gl.TEXTURE_2D, rd.tex);
  gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
  gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, source);
  if( source.tex && source.tex.GL_TEXTURE_MAG_FILTER == "GL_NEAREST" ) {
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
  } else {
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
  }
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
  gl.bindTexture(gl.TEXTURE_2D, null);

  if( source.prevWidth != source.width || source.prevHeight != source.height ) {
    var w = source.width;
    var h = source.height;
    var a = w/h;
    source.prevWidth = w;
    source.prevHeight = h
    $(parent).resizable({
      aspectRatio: a
    });
    var options = $(parent).resizable("option");
    w = Math.max( options.minWidth, Math.min( options.maxWidth, w ) );
    if( a < 1 ) {
      w *= a;
    }
    h = w/a;
    $(parent).animate( { width: w, height: h }, 200 );
    renderImage( img, { width: w, height: h } );
  } else {
    renderImage( img );
  }
  
  $(parent).resizable();

}

function restOfContainerWidth( jel, parentWidth ) {
  var w = 0;
  jel.siblings().each( function(index, el) {
    w += $(el).outerWidth(true);
  });
  var pw = parentWidth || jel.parent().width();
  var myouter = jel.outerWidth(true) - jel.width();
  var resizeMarginRight = jel[0].resizeMarginRight || 0;
  jel.width( pw - w - myouter - 2 - resizeMarginRight );
}

function renderImage( img, size ) {
  var render = $("#" + img + "render")[0];
  var source = $("#" + img + "source")[0];
  var canvas = $("#" + img + "canvas")[0];
  var slider = $("#" + img + "opacity");
  var gl = canvas.gl || ( canvas.gl = canvas.getContext('webgl') || canvas.getContext("experimental-webgl" ) );
  var parent = source.parentNode;

  var w = parent.clientWidth;
  var h = parent.clientHeight;
  if( size ) {
    w = size.width;
    h = size.height;
  }
  
  if( w <= 0 || h <= 0 ) {
    return;
  }

  var opacity = slider.slider("option", "value") / 100.0;
  canvas.width = render.width = w;
  canvas.height = render.height = h;


  gl.viewport( 0, 0, w, h );
  gl.clearColor( 1, 0, 0, 0 );
  gl.clear( gl.COLOR_BUFFER_BIT );

  var rd = canvas.renderData || ( canvas.renderData = new Object() );
  if( ! rd.vb ) {
    rd.vb = gl.createBuffer();
    gl.bindBuffer( gl.ARRAY_BUFFER, rd.vb );
    gl.bufferData( gl.ARRAY_BUFFER, new Float32Array( [ -1, -1, 1, -1, 1, 1, -1, -1, 1, 1, -1, 1 ] ), gl.STATIC_DRAW);
    rd.vs = gl.createShader( gl.VERTEX_SHADER );
    gl.shaderSource( rd.vs, 
        "attribute vec2 p;\n" +
        "varying vec2 tc;\n" +
        "void main() {\n" +
        "   tc = p * 0.5 + 0.5;\n" +
        "   gl_Position = vec4( p.x, p.y, 0.0, 1.0 );\n" +
        "}\n" );
    gl.compileShader( rd.vs );
    if (!gl.getShaderParameter( rd.vs, gl.COMPILE_STATUS) ) {
      var infoLog = gl.getShaderInfoLog( rd.vs );
      alert("Error compiling shader:\n" + infoLog);
    }
    rd.fs = gl.createShader( gl.FRAGMENT_SHADER );
    gl.shaderSource( rd.fs, 
        "precision highp float;\n" +
        "uniform sampler2D tex;\n" +
        "uniform float opacity;\n" +
        "varying vec2 tc;\n" +
        "void main() {\n" +
        "   float checker = mod( dot( floor( vec2(gl_FragCoord.x,gl_FragCoord.y) / 8.0 ) , vec2( 1.0, 1.0 ) ) , 2.0 );\n" +
        "   checker = checker * 0.25 + 0.75;\n" +
        "   vec4 c = vec4( checker, checker, checker, 1.0 );\n" +
        "   vec4 t = texture2D( tex, tc );\n" +
        "   t.w = t.w + ( 1.0 - t.w ) * opacity;\n" +
        "   gl_FragColor.xyz = c.xyz * (1.0 - t.w) + t.xyz * t.w;\n" +
        "   gl_FragColor.w = 1.0;\n" +
        "}\n" );
    gl.compileShader( rd.fs );
    if (!gl.getShaderParameter( rd.fs, gl.COMPILE_STATUS) ) {
      var infoLog = gl.getShaderInfoLog( rd.fs );
      alert("Error compiling shader:\n" + infoLog);
    }
    rd.prog = gl.createProgram();
    gl.attachShader( rd.prog, rd.vs );
    gl.attachShader( rd.prog, rd.fs );
    gl.linkProgram( rd.prog );
    gl.bindAttribLocation( rd.prog, 0, "p" );
    gl.linkProgram( rd.prog );
    gl.useProgram( rd.prog );
    gl.uniform1i( gl.getUniformLocation( rd.prog, "tex" ), 0 );
    rd.opacityLoc = gl.getUniformLocation( rd.prog, "opacity" );
  }

  if( ! rd.tex ) {
    return;
  }

  gl.activeTexture( gl.TEXTURE0 );
  gl.bindTexture( gl.TEXTURE_2D, rd.tex );
  gl.bindBuffer( gl.ARRAY_BUFFER, rd.vb );
  gl.vertexAttribPointer(0, 2, gl.FLOAT, false, 0, 0);
  gl.enableVertexAttribArray( 0 );
  gl.useProgram( rd.prog );
  gl.uniform1f( rd.opacityLoc, opacity );
  gl.drawArrays( gl.TRIANGLES, 0, 6 );


  $(render).attr( "src", canvas.toDataURL() );
  restOfContainerWidth( slider, w );
  var filler = $("#" + img ).parent().children(".restOfWidth");
  filler.css( { height: h } );
  restOfContainerWidth( filler );
}

function fetch( url, callback ) {
  $.ajax( {
    url: url,
    success: callback,
    error: function( data ) {
      console.log( "error! <br><br>" + data );
    } 
  } );
}

function updateLogData() {
  fetch( "/log/-100", function( logdata ) {
    $("#log").html( logdata.log.join( "<br>" ) );
    $("#log").scrollTop( 100000 );
  } );
}

function createDebugButton( id, label ) {
  var b = $('<button>', { id: id });
  $(b).button( { label: label } );
  $(b).click( function() { 
    $.get( "/debug/" + id, function() { 
      updateLogData();
      if( label !== "play" ) {
        $("#color0source")[0].onload = function() { imageLoaded( "color0" ); };
        $("#color0source").attr( "src", "/fbo/0/color0?" + new Date().getTime() );
      }
    } );
  } );
  return b;
}

function createDebugToolbar() {
  var t = $('<div>', { id: "debugToolbar", class: "ui-widget-header ui-corner-all" });
  t.append( createDebugButton("play", "play" ) );
  t.append( createDebugButton("nextFrame", "next frame" ) );
  t.append( createDebugButton("nextGroup", "next group" ) );
  t.append( createDebugButton("nextFbo", "next fbo" ) );
  t.append( createDebugButton("nextDraw", "next draw" ) );
  t.append( createDebugButton("next", "next" ) );
  return t;
}

function createSlider( parent, name ) {
  var sc = $('<div>');
  parent.append(sc);
  var pname = parent.attr("id");
  var label = $('<div>');
  label.text( name );
  var sl = $('<div>', { id: pname + name } );
  sc.append( label );
  sc.append( sl );
  sl.on( "slide", function( event, ui ) { renderImage( pname ); } );
  sl.slider( { value: 50 } );
  sc.children().css( {
    verticalAlign: "middle",
    margin: "5px",
    display: "inline-block",
  } );
}

function createImageView( p, name, url ) {
  var imgView = $('<div>', { id: name } );
  p.append( imgView );
  var div = $('<div>', { id: name + "div" } );
  var source = $('<img>', { id: name + "source", src: url } );
  var canvas = $('<canvas>', { id: name + "canvas", hidden: true } );
  var render = $('<img>', { id: name + "render" } );
  source.css("display", "none");
  imgView.append(div);
  div.append( source );
  div.append( canvas );
  div.append( render );
  div.resizable( { 
    aspectRatio: 1,
    maxWidth: 512,
    maxHeight: 512,
    minWidth: 128,
    minHeight: 128,
  } );
  div.on("resize", function( event, ui ) { renderImage(name, ui.size); } );

  createSlider( imgView, "opacity" );
  source.on( "load", function() { imageLoaded( name ); } );
}

function createDebug() {
  var debug = $("#debug");
  debug.append( createDebugToolbar() );
  var debugArea = $('<div>', { id: "debugArea" } );
  debug.append( debugArea );

  createImageView( debugArea, "color0", "/fbo/0/color0" );
  var log = $('<div>', { id: "log", class: "restOfWidth" } );
  log.css( { 
    height: 256, 
    overflowX: "hide",
    overflowY: "auto", 
    marginRight: "10px", 
    marginLeft: "10px", 
    fontSize: "80%",
    fontFamily: "Courier New, Courier, monospace",
  } );
  debugArea.append( log );
  debugArea.children().css( {
    verticalAlign: "top",
    margin: "5px",
    display: "inline-block",
  } );
  debug.css( {
    overflowX: "auto",
    overflowY: "auto",
  } );
 
  return debug;
}

function appendObjectItems( obj ) {
  var ul = $('<ul>');
  for( var k in obj ) {
    if( obj.hasOwnProperty(k) ) {
      var li = $('<li>');
      var span = $('<span>', { class: "key", text: k } );
      var sp = $('<span>', { html: "&nbsp;:&nbsp;" } );
      if( typeof obj[k] == "object" ) {
        var child = appendObjectItems( obj[k] );
        span.click( function() { $(this).next().next().toggle(); } );
      } else {
        var child = $('<span>', { text: obj[k] } );
      }
      li.append( span );
      li.append( sp );
      li.append( child );
      ul.append( li );
    }
  }
  return ul;
}

function loadTextureView( view, tex ) {
  view.empty();
  createImageView( view, "texView", "/texture/" + tex.name + "/image" );
  $("#texViewsource")[0].tex = tex;
  var texProps = appendObjectItems( tex );
  texProps.addClass( "restOfWidth" );
  view.append( texProps );
  texProps.css( {
    overflowX: "auto",
    overflowY: "auto",
    fontSize: "80%",
    fontFamily: "Courier New, Courier, monospace",
  } );
  var keys = texProps.find(".key");
  keys.css( {
    color: "yellow",
  } );
}

function createTextures() {
  var textures = $("#textures");

  var list = $('<div>', { id: "list", class: "ui-widget-header ui-corner-all" } );
  list.css( {
    padding: "5px",
  } );
  textures.append( list );
  var texInfo = $('<div>', { id: "texInfo" } );
  textures.append( texInfo );
  fetch( "/texture", function( d ) {
    var spans = new Array();
    d.forEach( function( t ) { spans.push(  "<span>" + t + "</span>" ); } );
    list.html( "Textures: " + spans.join( ",&nbsp; " ) );
    var spans = $("#textures #list span");
    spans.click( function( ev ) {
      fetch( "/texture/" + $(this).text(), function ( tex ) {
        loadTextureView( texInfo, tex );
        texInfo.children().css( {
          verticalAlign: "top",
          margin: "5px",
          display: "inline-block",
        } );
        texInfo.css( {
          overflowX: "auto",
          overflowY: "auto",
        } );
      } );
    } );
    spans.mouseenter( function() { $(this).css( { textDecoration: "underline" } ); } );
    spans.mouseleave( function() { $(this).css( { textDecoration: "none" } ); } );
  } );
}

function loadProgView( view, prog ) {
  view.empty();
  var progProps = appendObjectItems( prog );
  view.append( progProps );
  progProps.css( {
    overflowX: "auto",
    overflowY: "auto",
    fontSize: "80%",
    fontFamily: "Courier New, Courier, monospace",
  } );
  var keys = progProps.find(".key");
  keys.css( {
    color: "yellow",
  } );
}

function createPrograms() {
  var programs = $("#programs");

  var list = $('<div>', { id: "list", class: "ui-widget-header ui-corner-all" } );
  list.css( { padding: "5px", } );
  programs.append( list );
  var progInfo = $('<div>', { id: "progInfo" } );
  programs.append( progInfo );
  fetch( "/program", function( d ) {
    var spans = new Array();
    d.forEach( function( t ) { spans.push(  "<span>" + t + "</span>" ); } );
    list.html( "Programs: " + spans.join( ",&nbsp; " ) );
    var spans = $("#programs #list span");
    spans.click( function( ev ) {
      fetch( "/program/" + $(this).text(), function ( prog ) {
        loadProgView( progInfo, prog );
        progInfo.children().css( {
          verticalAlign: "top",
          margin: "5px",
        } );
        progInfo.css( {
          overflowX: "auto",
          overflowY: "auto",
        } );
      } );
    } );
    spans.mouseenter( function() { $(this).css( { textDecoration: "underline" } ); } );
    spans.mouseleave( function() { $(this).css( { textDecoration: "none" } ); } );
  } );
}

function loadShaderView( view, shd ) {
  view.empty();
  var shdProps = appendObjectItems( shd );
  view.append( shdProps );
  shdProps.css( {
    overflowX: "auto",
    overflowY: "auto",
    fontSize: "80%",
    fontFamily: "Courier New, Courier, monospace",
  } );
  var keys = shdProps.find(".key");
  keys.css( {
    color: "yellow",
  } );
}

function createShaders() {
  var shaders = $("#shaders");

  var list = $('<div>', { id: "list", class: "ui-widget-header ui-corner-all" } );
  list.css( { padding: "5px", } );
  shaders.append( list );
  var shdInfo = $('<div>', { id: "shdInfo" } );
  shaders.append( shdInfo );
  fetch( "/shader", function( d ) {
    var spans = new Array();
    d.forEach( function( t ) { spans.push(  "<span>" + t + "</span>" ); } );
    list.html( "Programs: " + spans.join( ",&nbsp; " ) );
    var spans = $("#shaders #list span");
    spans.click( function( ev ) {
      fetch( "/shader/" + $(this).text(), function ( shd ) {
        loadProgView( shdInfo, shd );
        shdInfo.children().css( {
          verticalAlign: "top",
          margin: "5px",
        } );
        shdInfo.css( {
          overflowX: "auto",
          overflowY: "auto",
        } );
      } );
    } );
    spans.mouseenter( function() { $(this).css( { textDecoration: "underline" } ); } );
    spans.mouseleave( function() { $(this).css( { textDecoration: "none" } ); } );
  } );
}

function createGeneric( id ) {
  var panel = $("#" + id);
  panel.text( id );
}

function mainTabsBeforeActivate( event, ui ) {
  if( ! ui.newPanel || ui.newPanel.children().length > 0 ) {
    return;
  }
  var panel = ui.newPanel.attr("id");
  var create = "create" + panel.charAt(0).toUpperCase() + panel.slice(1);
  if( window.hasOwnProperty( create ) ) {
    window[create]();
  } else {
    createGeneric( panel );
  }
}

function createMainTab() {

  var m = $('<div>', { id: "maintabs" } );
  $(m).html(
      "<ul>\n" +
      "  <li><a href=\"#debug\">debug</a></li>\n" +
      "  <li><a href=\"#programs\">programs</a></li>\n" +
      "  <li><a href=\"#shaders\">shaders</a></li>\n" +
      "  <li><a href=\"#textures\">textures</a></li>\n" +
      "  <li><a href=\"#buffers\">buffers</a></li>\n" +
      "</ul>\n" +
      "" +
      "<div id=\"debug\"></div>\n" +
      "<div id=\"programs\"></div>\n" +
      "<div id=\"shaders\"></div>\n" +
      "<div id=\"textures\"></div>\n" +
      "<div id=\"buffers\"></div>\n" +
      ""
      );

  $("body").append(m);
  m.on( "tabsbeforeactivate", mainTabsBeforeActivate );
  createDebug();
  m.tabs();
  
  updateLogData();
  $(window).resize( function() {
    var log = $("#log")[0];
    log.resizeMarginRight = 80;
    if( log.resizeMarginRightTimeout ) {
      clearTimeout( log.resizeMarginRightTimeout );
      log.resizeMarginRightTimeout = 0;
    }
    log.resizeMarginRightTimeout = setTimeout( function() {
      log.resizeMarginRight = 0;
      log.resizeMarginRightTimeout = 0;
      renderImage("color0"); 
    }, 500 );
    renderImage("color0"); 
  } );
}

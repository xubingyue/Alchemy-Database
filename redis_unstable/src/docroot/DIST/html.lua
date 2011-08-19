
function create_navbar(my_userid)
  local domain = GetHttpDomainPort(my_userid)
  if (LoggedIn) then
    output([[<div id="navbar">
      <a href="]] .. build_link(my_userid, 'home')     .. [[">home</a> |
      <a href="]] .. build_link(my_userid, 'timeline') .. [[">timeline</a> |
      <a href="]] .. build_link(my_userid, 'logout', my_userid) .. 
                                                          '">logout</a></div>');
  else
    output([[<div id="navbar">
      <a href="]] .. build_link(my_userid, 'index_page') .. [[">home</a> |
      <a href="]] .. build_link(my_userid, 'timeline')   ..
                                                        '">timeline</a></div>');
  end
end

function create_header(inline, my_userid)
  local script;
  if (inline) then script = '<script>var AlchemyStartLoad=new Date();</script>';
  else             script = ''; end
  output(
[[
<html>
<head>
<meta content="text/html; charset=UTF-8" http-equiv="content-type" />
]]                                                        ..
    script                                                ..
    inline_include_js_at_EOD("STATIC/helper.js")          ..
    inline_include_css      ("STATIC/css/style.css")      ..
[[
<link rel="shortcut icon" href="/STATIC/favicon.ico" />
<title>Retwis - Example Twitter clone based on Alchemy DB</title>
</head>
<body>
<div id="page">
<div id="header">
]] ..
  inline_include_png_at_EOD(
               '<img style="border:none" width="192" height="85" alt="Retwis" ',
               'STATIC/logo.png'));
  create_navbar(my_userid);
  output('</div>');
end

function create_inlined_js_for_png_at_EOD(ihtml_beg, n, isrc)
  local b64_body = redis("get", 'BASE64/' .. isrc);
  return 'document.getElementById("alc_png_eod_" + ' .. n .. ').innerHTML = ' ..
           [[']] .. ihtml_beg .. [[ src="' + ]] ..
                 [['data:image/png;base64,]] .. b64_body .. [[' + '">'; ]];
end
function create_post_onload_script(inline)
  if (inline) then -- create JS script to post_load all inlined elements
    local now_js = '';
    for k,v in pairs(InlinedPNG_EOD) do
      local ihtml_beg = v[1];
      local isrc      = v[2];
      now_js = now_js .. create_inlined_js_for_png_at_EOD(ihtml_beg, k, isrc);
    end
    for k,v in pairs(InlinedJS_EOD) do
      local body = redis("get", v)
      now_js     = now_js  .. body;
    end --print ('now_js: ' .. now_js);
    local post_load_js = '';
    for k,v in pairs(InlinedJS)  do
      post_load_js = post_load_js  .. 'include_alc("' .. v .. '");';
    end
    local post_load_html = '';
    for k,v in pairs(InlinedCSS) do
      post_load_html = post_load_html .. '<link href="/' .. v ..
                                        '" rel="stylesheet" type="text/css" />';
    end
    for k,v in pairs(InlinedPNG) do
      post_load_html = post_load_html .. '<img src="/' .. v .. '" />';
    end
    for k,v in pairs(InlinedPNG_EOD) do
      local isrc = v[1];
      post_load_html = post_load_html .. '<img src="/' .. isrc .. '" />';
    end
    return [[
<script>

]] .. now_js     ..
[[

function window_loaded() {
  var now = new Date();
  var LoadStatsDiv = document.getElementById('load_stats');
  LoadStatsDiv.innerHTML = 'LOAD TIME: ' +
                           (now.getTime() - AlchemyStartLoad.getTime()) + ' ms';

]] .. post_load_js ..
[[

  var AlcPostLoadReload = document.getElementById('alchemy_postload_reload');
  AlcPostLoadReload.innerHTML = ']] .. post_load_html .. [[';

}
window.onload=function() { window_loaded(); }
</script>
]];
  else
    return '';
  end
end
function create_footer(inline)
  local post_onload_script = create_post_onload_script(inline);
  output([[
  <div id="footer">
    <a href="http://code.google.com/p/alchemydatabase/">Alchemy Database</a> is a A Hybrid Relational-Database/NOSQL-Datastore/Distributed-Web-Platform
  </div>
  <div id="load_stats"></div>
</div>
<div id="alchemy_postload_reload" style="display: none"></div>
]] .. post_onload_script .. [[
  <style type="text/css">
  BODY {
    font-family: Verdana, sans-serif;
    background: url(]] .. inline_include_png_src('STATIC/sfondo.png') ..
            [[) repeat-x top white;
    background-attachment: fixed;
  }
  </style>
</body> </html>
]]);
end

function create_welcome()
  output([[ 
<div id="welcomebox">
<div id="registerbox">
<h2>Register!</h2>
<b>Want to try Retwis? Create an account!</b>
<form action="/register" method="POST" onsubmit="return passwords_match(this.elements['password'].value, this.elements['password2'].value)">
<input type="hidden" name="IO_OP" value="w" />
<table>
<tr> <td>Username</td><td><input type="text" name="username"></td> </tr>
<tr> <td>Password</td><td><input type="password" name="password"></td> </tr>
<tr> <td>Password (again)</td><td><input type="password" name="password2"></td> </tr>
<tr> <td colspan="2" align="right"><input type="submit" name="doit" value="Create an account"></td> </tr>
</table>
</form>
<h2>Already registered? Login here</h2>
<form action="/login" method="POST" >
<input type="hidden" name="IO_OP" value="r" />
<table>
<tr> <td>Username</td><td><input type="text" name="username"></td> </tr>
<tr> <td>Password</td><td><input type="password" name="password"></td> </tr>
<tr> <td colspan="2" align="right"><input type="submit" name="doit" value="Login"></td> </tr>
</table>
</form>
</div>
Hello! Retwis is a very simple clone of <a href="http://twitter.com">Twitter</a>, as a demo for the <a href="http://code.google.com/p/alchemydatabase/wiki/ShortStack">Alchemy's Short-Stack</a>
</div>
]]);
end

function create_home_post_box()
  return [[
<br>
<table>
<tr><td><textarea cols="70" rows="3" name="status"></textarea></td></tr>
<tr><td align="right"><input type="submit" name="doit" value="Update"></td></tr>
</table>
</form>
]];
end

function create_follow(my_userid) -- Button controld by Cookie sent w/ Response
output([[
<script>
  var cooks     = process_cookies();
  var following = cooks['following'];
  var userid    = cooks['other_userid'];
  if (typeof following=="undefined" || typeof userid=="undefined") { return; }
  var url_beg   = ']] .. build_link(my_userid, "follow", my_userid) .. [[';
  if        (following == 0) {
    document.write('<a href="' + url_beg + '/' + userid + '/1" class="button">Follow this user</a>');
  } else if (following == 1) {
    document.write('<a href="' + url_beg + '/' + userid + '/0" class="button">Stop following</a>');
  }
  //document.write("<br/><br/>following: " + following + " userid: " + userid + "<br/>");
</script>
]]);
end

function scriptElapsed(t)
  return '<script> output_elapsed(' .. t .. ');</script>';
end

function showPost(id)
  local postdata = redis("get", "post:" .. id);
  if (postdata == nil) then return false; end
  local aux      = explode("|", postdata);
  local userid   = aux[1];
  local time     = aux[2];
  local username = redis("get", "uid:" .. userid .. ":username");
  local post     = aux[3];
  local userlink = 
  output([[
    <div class="post">
      <a class="username" href="]] .. build_link(userid, "profile", userid) ..  
      '">' ..  username .. "</a>" ..  ' ' .. post .."<br>" .. '<i>posted '..
           scriptElapsed(time) ..' ago via web</i></div>');
  return true;
end

function showUserPosts(key, start, count)
  output([[
<script>
var AlchemyNow  = new Date();
var AlchemyNows = (AlchemyNow.getTime()/1000);
</script>
]]);
  local posts = redis("lrange", key, start, (start + count));
  local c     = 0;
  for k,v in pairs(posts) do
      if (showPost(v)) then c = c + 1; end
      if (c == count) then break; end
  end
end

function showUserPostsWithPagination(page, nposts, username, userid,
                                     start, count)
  local navlink  = "";
  local nextc    = start + 10;
  local prevc    = start - 10;
  local nextlink = "";
  local prevlink = "";
  if (prevc < 0) then prevc = 0; end
  local key, u;
  if (username) then u   = userid; key = "uid:" .. userid .. ":myposts";
  else               u   = 0;      key = "uid:" .. userid .. ":posts"; end
  showUserPosts(key, start, count);
  if (nposts ~= nil and nposts > start + count) then
      nextlink = '<a href="' .. build_link(userid, page, u, nextc) ..
                         '">&raquo; Older posts </a>';
  end
  if (start > 0) then
      prevlink = '<a href="' .. build_link(userid, page, u, prevc) ..
                        '">Newer posts &laquo;</a>';
  end
  local divider;
  if (string.len(nextlink) and string.len(prevlink)) then divider = ' --- ';
  else                                                    divider = ' '; end
  if (string.len(nextlink) or string.len(prevlink)) then
      output('<div class="rightlink">' .. prevlink .. divider ..
                                          nextlink .. '</div>');
  end
end

function showLastUsers()
  local users = redis("sort", "global:users", "GET", "uid:*:username",
                      "DESC", "LIMIT",        0,     10);
  output('<div>');
  for k,v in pairs(users) do
    local userid = redis("get", "username:" .. v .. ":id");
    output('<a class="username" href="' ..
                build_link(userid, "profile", userid) ..  '">' .. v .. '</a> ');
  end
  output('</div><br>');
end

function create_home(my_userid, my_username, start, nposts,
                     nfollowers, nfollowing)
  local page = 'home';
  local s    = 0;
  if (start ~= nil) then s = tonumber(start); end
  output('<div id="postform"><form action="/post/w/' .. my_userid ..
                              '" method="POST">');
  output(my_username ..', what you are doing?');

  output(create_home_post_box());
  output('<div id="homeinfobox">' .. nfollowers .. " followers<br>" ..
                                    nfollowing .. " following<br></div></div>");
  showUserPostsWithPagination(page, nposts, false, my_userid, s, 10);
end

function goback(my_userid, msg)
  create_header(false, my_userid);
  output('<div id ="error">' .. msg ..
     '<br><a href="javascript:history.back()">Go back and try again</a></div>');
  create_footer(false);
end
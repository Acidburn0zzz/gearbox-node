#line 1 "src/modules/tty.gear"

// Copyright Joyent, Inc. and other Node contributors.
//           (c) 2011 the gearbox-node project authors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <gearbox.h>

using namespace Gearbox;

/** \file src/modules/tty.cc converted from src/modules/tty.gear */

#line 25 "src/modules/tty.gear"

#include <assert.h>
#include <unistd.h>
#include <UvCommon.h>

struct _tty_tty_wrap_TTY_wrap /*: public Value::DtorWrap*/ {
    uv_tty_t handle;
    bool unref;
    size_t slabOffset;

    struct This : public Value {
        This(v8::Handle<v8::Object> &&_this, _tty_tty_wrap_TTY_wrap *wrap) : Value(_this), _wrap(wrap), handle(wrap->handle), unref(wrap->unref), slabOffset(wrap->slabOffset) {
            _this->SetPointerInInternalField(0, wrap);
        }
        This(v8::Handle<v8::Object> &&_this) : Value(_this), _wrap(static_cast<_tty_tty_wrap_TTY_wrap*>(_this->GetPointerFromInternalField(0))), handle(_wrap->handle), unref(_wrap->unref), slabOffset(_wrap->slabOffset) {}
        _tty_tty_wrap_TTY_wrap *_wrap;
        uv_tty_t &handle;
        bool &unref;
        size_t &slabOffset;
    };
};

static v8::Handle<v8::Value> _tty_tty_wrap_TTY_TTY(const v8::Arguments &args) {
    _tty_tty_wrap_TTY_wrap::This This(args.This(), new _tty_tty_wrap_TTY_wrap);
    if(args.Length() >= 2) {
        #line 38 "src/modules/tty.gear"
        Value fd(args[0]), readable(args[1]);
        This.unref = false;
                uv_tty_init(uv_default_loop(), &This.handle, fd, readable);
                
                //FIXME Will This even work? It would never be cleaned up.
                This.handle.data = new decltype(This)(This);
                Uv::Stream::updateWriteQueueSize(This);
        return undefined;
    }
    THROW_ERROR("Invalid call to tty.tty.wrap.TTY");
}

static v8::Handle<v8::Value> _tty_tty_wrap_TTY_unref(const v8::Arguments &args) {
    _tty_tty_wrap_TTY_wrap::This This(args.This());
    #line 48 "src/modules/tty.gear"
    Uv::Handle::unref(This);
    return undefined;
}

static v8::Handle<v8::Value> _tty_tty_wrap_TTY_close(const v8::Arguments &args) {
    _tty_tty_wrap_TTY_wrap::This This(args.This());
    #line 50 "src/modules/tty.gear"
    Uv::Handle::close(This);
    return undefined;
}

static v8::Handle<v8::Value> _tty_tty_wrap_TTY_shutdown(const v8::Arguments &args) {
    _tty_tty_wrap_TTY_wrap::This This(args.This());
    #line 53 "src/modules/tty.gear"
    std::cerr << "TODO: TTY shutdown" << std::endl;
    return undefined;
}

static v8::Handle<v8::Value> _tty_tty_wrap_TTY_readStart(const v8::Arguments &args) {
    _tty_tty_wrap_TTY_wrap::This This(args.This());
    #line 57 "src/modules/tty.gear"
    Uv::Stream::readStart(This);
    return undefined;
}

static v8::Handle<v8::Value> _tty_tty_wrap_TTY_readStop(const v8::Arguments &args) {
    _tty_tty_wrap_TTY_wrap::This This(args.This());
    #line 59 "src/modules/tty.gear"
    Uv::Stream::readStop(This);
    return undefined;
}

static v8::Handle<v8::Value> _tty_tty_wrap_TTY_write(const v8::Arguments &args) {
    _tty_tty_wrap_TTY_wrap::This This(args.This());
    if(args.Length() >= 4) {
        #line 61 "src/modules/tty.gear"
        Value buffer(args[0]), offset(args[1]), length(args[2]), sendStream(args[3]);
        return Uv::Stream::write(This, buffer, offset, length, sendStream);
    }

    if(args.Length() >= 3) {
        #line 63 "src/modules/tty.gear"
        Value buffer(args[0]), offset(args[1]), length(args[2]);
        return Uv::Stream::write(This, buffer, offset, length);
    }

    if(args.Length() >= 2) {
        #line 65 "src/modules/tty.gear"
        Value buffer(args[0]), offset(args[1]);
        return Uv::Stream::write(This, buffer, offset);
    }

    if(args.Length() >= 1) {
        #line 67 "src/modules/tty.gear"
        Value buffer(args[0]);
        return Uv::Stream::write(This, buffer);
    }
    THROW_ERROR("Invalid call to tty.tty.wrap.TTY.prototype.write");
}

static v8::Handle<v8::Value> _tty_tty_wrap_isTTY(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 71 "src/modules/tty.gear"
        Value fd(args[0]);
        return Value(uv_guess_handle(fd) == UV_TTY);
    }
    THROW_ERROR("Invalid call to tty.tty.wrap.isTTY");
}

static v8::Handle<v8::Value> _tty_tty_wrap_guessHandleType(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 74 "src/modules/tty.gear"
        Value _fd(args[0]);
        int fd = _fd;
            assert(fd >= 0);
            
            uv_handle_type t = uv_guess_handle(fd);
            
            switch(t) {
        case UV_TTY:
            return String("TTY");
        case UV_NAMED_PIPE:
            return String("PIPE");
        case UV_FILE:
            return String("FILE");
        default:
            assert(0);
            }
        return undefined;
    }
    THROW_ERROR("Invalid call to tty.tty.wrap.guessHandleType");
}


#line 167 "src/modules/tty.cc"
static void _setup_tty(Value exports, Value require, Value module) {
    var tty_wrap = Object();
    v8::Handle<v8::FunctionTemplate> _tty_tty_wrap_TTY = v8::FunctionTemplate::New(_tty_tty_wrap_TTY_TTY);
    _tty_tty_wrap_TTY->SetClassName(String("TTY"));
    _tty_tty_wrap_TTY->InstanceTemplate()->SetInternalFieldCount(1);
    _tty_tty_wrap_TTY->PrototypeTemplate()->Set("unref", Function(_tty_tty_wrap_TTY_unref, "unref"));
    _tty_tty_wrap_TTY->PrototypeTemplate()->Set("close", Function(_tty_tty_wrap_TTY_close, "close"));
    _tty_tty_wrap_TTY->PrototypeTemplate()->Set("shutdown", Function(_tty_tty_wrap_TTY_shutdown, "shutdown"));
    _tty_tty_wrap_TTY->PrototypeTemplate()->Set("readStart", Function(_tty_tty_wrap_TTY_readStart, "readStart"));
    _tty_tty_wrap_TTY->PrototypeTemplate()->Set("readStop", Function(_tty_tty_wrap_TTY_readStop, "readStop"));
    _tty_tty_wrap_TTY->PrototypeTemplate()->Set("write", Function(_tty_tty_wrap_TTY_write, "write"));
    tty_wrap["TTY"] = _tty_tty_wrap_TTY->GetFunction();
    tty_wrap["isTTY"] = Function(_tty_tty_wrap_isTTY, "isTTY");
    tty_wrap["guessHandleType"] = Function(_tty_tty_wrap_guessHandleType, "guessHandleType");
    Context::getCurrent()->runScript("(function(exports, require, module, tty_wrap){\n//BEGIN lib/tty.js\nvar assert = require('assert');\nvar inherits = require('util').inherits;\nvar net = require('net');\n//BEGIN *gearbox\n//var TTY = process.binding('tty_wrap').TTY;\n//var isTTY = process.binding('tty_wrap').isTTY;\nvar TTY = tty_wrap.TTY;\nvar isTTY = tty_wrap.isTTY;\nexports.guessHandleType = tty_wrap.guessHandleType;\n//END *gearbox\n\nvar stdinHandle;\n\n\nexports.isatty = function(fd) {\n  return isTTY(fd);\n};\n\n\nexports.setRawMode = function(flag) {\n  assert.ok(stdinHandle, 'stdin must be initialized before calling setRawMode');\n  stdinHandle.setRawMode(flag);\n};\n\n\nexports.getWindowSize = function() {\n  //throw new Error(\"implement me\");\n  return 80;\n};\n\n\nexports.setWindowSize = function() {\n  throw new Error('implement me');\n};\n\n\nfunction ReadStream(fd) {\n  if (!(this instanceof ReadStream)) return new ReadStream(fd);\n  net.Socket.call(this, {\n    handle: new TTY(fd, true)\n  });\n\n  this.writable = false;\n\n  var self = this,\n      keypressListeners = this.listeners('keypress');\n\n  function onData(b) {\n    if (keypressListeners.length) {\n      self._emitKey(b);\n    } else {\n      // Nobody's watching anyway\n      self.removeListener('data', onData);\n      self.on('newListener', onNewListener);\n    }\n  }\n\n  function onNewListener(event) {\n    if (event == 'keypress') {\n      self.on('data', onData);\n      self.removeListener('newListener', onNewListener);\n    }\n  }\n\n  if (!stdinHandle) stdinHandle = this._handle;\n\n  this.on('newListener', onNewListener);\n}\ninherits(ReadStream, net.Socket);\nexports.ReadStream = ReadStream;\n\n\nReadStream.prototype.isTTY = true;\n\n\n/*\n  Some patterns seen in terminal key escape codes, derived from combos seen\n  at http://www.midnight-commander.org/browser/lib/tty/key.c\n\n  ESC letter\n  ESC [ letter\n  ESC [ modifier letter\n  ESC [ 1 ; modifier letter\n  ESC [ num char\n  ESC [ num ; modifier char\n  ESC O letter\n  ESC O modifier letter\n  ESC O 1 ; modifier letter\n  ESC N letter\n  ESC [ [ num ; modifier char\n  ESC [ [ 1 ; modifier letter\n  ESC ESC [ num char\n  ESC ESC O letter\n\n  - char is usually ~ but $ and ^ also happen with rxvt\n  - modifier is 1 +\n                (shift     * 1) +\n                (left_alt  * 2) +\n                (ctrl      * 4) +\n                (right_alt * 8)\n  - two leading ESCs apparently mean the same as one leading ESC\n*/\n\n\n// Regexes used for ansi escape code splitting\nvar metaKeyCodeRe = /^(?:\\x1b)([a-zA-Z0-9])$/;\nvar functionKeyCodeRe =\n    /^(?:\\x1b+)(O|N|\\[|\\[\\[)(?:(\\d+)(?:;(\\d+))?([~^$])|(?:1;)?(\\d+)?([a-zA-Z]))/;\n\n\nReadStream.prototype._emitKey = function(s) {\n  var char,\n      key = {\n        name: undefined,\n        ctrl: false,\n        meta: false,\n        shift: false\n      },\n      parts;\n\n  if (Buffer.isBuffer(s)) {\n    if (s[0] > 127 && s[1] === undefined) {\n      s[0] -= 128;\n      s = '\\x1b' + s.toString(this.encoding || 'utf-8');\n    } else {\n      s = s.toString(this.encoding || 'utf-8');\n    }\n  }\n\n  if (s === '\\r' || s === '\\n') {\n    // enter\n    key.name = 'enter';\n\n  } else if (s === '\\t') {\n    // tab\n    key.name = 'tab';\n\n  } else if (s === '\\b' || s === '\\x7f' ||\n             s === '\\x1b\\x7f' || s === '\\x1b\\b') {\n    // backspace or ctrl+h\n    key.name = 'backspace';\n    key.meta = (s.charAt(0) === '\\x1b');\n\n  } else if (s === '\\x1b' || s === '\\x1b\\x1b') {\n    // escape key\n    key.name = 'escape';\n    key.meta = (s.length === 2);\n\n  } else if (s === ' ' || s === '\\x1b ') {\n    key.name = 'space';\n    key.meta = (s.length === 2);\n\n  } else if (s <= '\\x1a') {\n    // ctrl+letter\n    key.name = String.fromCharCode(s.charCodeAt(0) + 'a'.charCodeAt(0) - 1);\n    key.ctrl = true;\n\n  } else if (s.length === 1 && s >= 'a' && s <= 'z') {\n    // lowercase letter\n    key.name = s;\n\n  } else if (s.length === 1 && s >= 'A' && s <= 'Z') {\n    // shift+letter\n    key.name = s.toLowerCase();\n    key.shift = true;\n\n  } else if (parts = metaKeyCodeRe.exec(s)) {\n    // meta+character key\n    key.name = parts[1].toLowerCase();\n    key.meta = true;\n    key.shift = /^[A-Z]$/.test(parts[1]);\n\n  } else if (parts = functionKeyCodeRe.exec(s)) {\n    // ansi escape sequence\n\n    // reassemble the key code leaving out leading \\x1b's,\n    // the modifier key bitflag and any meaningless \"1;\" sequence\n    var code = (parts[1] || '') + (parts[2] || '') +\n               (parts[4] || '') + (parts[6] || ''),\n        modifier = (parts[3] || parts[5] || 1) - 1;\n\n    // Parse the key modifier\n    key.ctrl = !!(modifier & 4);\n    key.meta = !!(modifier & 10);\n    key.shift = !!(modifier & 1);\n\n    // Parse the key itself\n    switch (code) {\n      /* xterm/gnome ESC O letter */\n      case 'OP': key.name = 'f1'; break;\n      case 'OQ': key.name = 'f2'; break;\n      case 'OR': key.name = 'f3'; break;\n      case 'OS': key.name = 'f4'; break;\n\n      /* xterm/rxvt ESC [ number ~ */\n      case '[11~': key.name = 'f1'; break;\n      case '[12~': key.name = 'f2'; break;\n      case '[13~': key.name = 'f3'; break;\n      case '[14~': key.name = 'f4'; break;\n\n      /* common */\n      case '[15~': key.name = 'f5'; break;\n      case '[17~': key.name = 'f6'; break;\n      case '[18~': key.name = 'f7'; break;\n      case '[19~': key.name = 'f8'; break;\n      case '[20~': key.name = 'f9'; break;\n      case '[21~': key.name = 'f10'; break;\n      case '[23~': key.name = 'f11'; break;\n      case '[24~': key.name = 'f12'; break;\n\n      /* xterm ESC [ letter */\n      case '[A': key.name = 'up'; break;\n      case '[B': key.name = 'down'; break;\n      case '[C': key.name = 'right'; break;\n      case '[D': key.name = 'left'; break;\n      case '[E': key.name = 'clear'; break;\n      case '[F': key.name = 'end'; break;\n      case '[H': key.name = 'home'; break;\n\n      /* xterm/gnome ESC O letter */\n      case 'OA': key.name = 'up'; break;\n      case 'OB': key.name = 'down'; break;\n      case 'OC': key.name = 'right'; break;\n      case 'OD': key.name = 'left'; break;\n      case 'OE': key.name = 'clear'; break;\n      case 'OF': key.name = 'end'; break;\n      case 'OH': key.name = 'home'; break;\n\n      /* xterm/rxvt ESC [ number ~ */\n      case '[1~': key.name = 'home'; break;\n      case '[2~': key.name = 'insert'; break;\n      case '[3~': key.name = 'delete'; break;\n      case '[4~': key.name = 'end'; break;\n      case '[5~': key.name = 'pageup'; break;\n      case '[6~': key.name = 'pagedown'; break;\n\n      /* putty */\n      case '[[5~': key.name = 'pageup'; break;\n      case '[[6~': key.name = 'pagedown'; break;\n\n      /* rxvt */\n      case '[7~': key.name = 'home'; break;\n      case '[8~': key.name = 'end'; break;\n\n      /* rxvt keys with modifiers */\n      case '[a': key.name = 'up'; key.shift = true; break;\n      case '[b': key.name = 'down'; key.shift = true; break;\n      case '[c': key.name = 'right'; key.shift = true; break;\n      case '[d': key.name = 'left'; key.shift = true; break;\n      case '[e': key.name = 'clear'; key.shift = true; break;\n\n      case '[2$': key.name = 'insert'; key.shift = true; break;\n      case '[3$': key.name = 'delete'; key.shift = true; break;\n      case '[5$': key.name = 'pageup'; key.shift = true; break;\n      case '[6$': key.name = 'pagedown'; key.shift = true; break;\n      case '[7$': key.name = 'home'; key.shift = true; break;\n      case '[8$': key.name = 'end'; key.shift = true; break;\n\n      case 'Oa': key.name = 'up'; key.ctrl = true; break;\n      case 'Ob': key.name = 'down'; key.ctrl = true; break;\n      case 'Oc': key.name = 'right'; key.ctrl = true; break;\n      case 'Od': key.name = 'left'; key.ctrl = true; break;\n      case 'Oe': key.name = 'clear'; key.ctrl = true; break;\n\n      case '[2^': key.name = 'insert'; key.ctrl = true; break;\n      case '[3^': key.name = 'delete'; key.ctrl = true; break;\n      case '[5^': key.name = 'pageup'; key.ctrl = true; break;\n      case '[6^': key.name = 'pagedown'; key.ctrl = true; break;\n      case '[7^': key.name = 'home'; key.ctrl = true; break;\n      case '[8^': key.name = 'end'; key.ctrl = true; break;\n\n      /* misc. */\n      case '[Z': key.name = 'tab'; key.shift = true; break;\n\n    }\n  } else if (s.length > 1 && s[0] !== '\\x1b') {\n    // Got a longer-than-one string of characters.\n    // Probably a paste, since it wasn't a control sequence.\n    Array.prototype.forEach.call(s, this._emitKey, this);\n    return;\n  }\n\n  // Don't emit a key if no name was found\n  if (key.name === undefined) {\n    key = undefined;\n  }\n\n  if (s.length === 1) {\n    char = s;\n  }\n\n  if (key || char) {\n    this.emit('keypress', char, key);\n  }\n};\n\n\nfunction WriteStream(fd) {\n  if (!(this instanceof WriteStream)) return new WriteStream(fd);\n  net.Socket.call(this, {\n    handle: new TTY(fd, false)\n  });\n\n  this.readable = false;\n  this.writable = true;\n}\ninherits(WriteStream, net.Socket);\nexports.WriteStream = WriteStream;\n\n\nWriteStream.prototype.isTTY = true;\n\n\nWriteStream.prototype.cursorTo = function(x, y) {\n  if (typeof x !== 'number' && typeof y !== 'number')\n    return;\n\n  if (typeof x !== 'number')\n    throw new Error(\"Can't set cursor row without also setting it's column\");\n\n  if (typeof y !== 'number') {\n    this.write('\\x1b[' + (x + 1) + 'G');\n  } else {\n    this.write('\\x1b[' + (y + 1) + ';' + (x + 1) + 'H');\n  }\n};\n\n\nWriteStream.prototype.moveCursor = function(dx, dy) {\n  if (dx < 0) {\n    this.write('\\x1b[' + (-dx) + 'D');\n  } else if (dx > 0) {\n    this.write('\\x1b[' + dx + 'C');\n  }\n\n  if (dy < 0) {\n    this.write('\\x1b[' + (-dy) + 'A');\n  } else if (dy > 0) {\n    this.write('\\x1b[' + dy + 'B');\n  }\n};\n\n\nWriteStream.prototype.clearLine = function(dir) {\n  if (dir < 0) {\n    // to the beginning\n    this.write('\\x1b[1K');\n  } else if (dir > 0) {\n    // to the end\n    this.write('\\x1b[0K');\n  } else {\n    // entire line\n    this.write('\\x1b[2K');\n  }\n};\n\n\nWriteStream.prototype.getWindowSize = function() {\n  return this._handle.getWindowSize();\n};\n//END lib/tty.js\n    })", "gear:tty")(exports, require, module, tty_wrap);
}
static NativeModule _module_tty("tty", _setup_tty);
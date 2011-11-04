// Copyright Joyent, Inc. and other Node contributors.
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

/** \file src/modules/readline.cc converted from src/modules/readline.js */

#line 1 "src/modules/readline.js"


#line 31 "src/modules/readline.js"
static void _setup_readline(Value exports, Value require, Value module) {
    Context::getCurrent()->runScript("(function(exports, require, module){\n\n// Inspiration for this code comes from Salvatore Sanfilippo's linenoise.\n// https://github.com/antirez/linenoise\n// Reference:\n// * http://invisible-island.net/xterm/ctlseqs/ctlseqs.html\n// * http://www.3waylabs.com/nw/WWW/products/wizcon/vt220.html\n\nvar kHistorySize = 30;\nvar kBufSize = 10 * 1024;\n\nvar util = require('util');\nvar inherits = require('util').inherits;\nvar EventEmitter = require('events').EventEmitter;\nvar tty = require('tty');\n\n\nexports.createInterface = function(input, output, completer) {\n  return new Interface(input, output, completer);\n};\n\n\nfunction Interface(input, output, completer) {\n  if (!(this instanceof Interface)) {\n    return new Interface(input, output, completer);\n  }\n  EventEmitter.call(this);\n\n  completer = completer || function() { return []; };\n\n  if (typeof completer !== 'function') {\n    throw new TypeError(\"Argument 'completer' must be a function\");\n  }\n\n  var self = this;\n\n  this.output = output;\n  this.input = input;\n  input.resume();\n\n  // Check arity, 2 - for async, 1 for sync\n  this.completer = completer.length === 2 ? completer : function(v, callback) {\n    callback(null, completer(v));\n  };\n\n  this.setPrompt('> ');\n\n  this.enabled = output.isTTY;\n\n  if (parseInt(process.env['NODE_NO_READLINE'], 10)) {\n    this.enabled = false;\n  }\n\n  if (!this.enabled) {\n    input.on('data', function(data) {\n      self._normalWrite(data);\n    });\n\n  } else {\n\n    // input usually refers to stdin\n    input.on('keypress', function(s, key) {\n      self._ttyWrite(s, key);\n    });\n\n    // Current line\n    this.line = '';\n\n    // Check process.env.TERM ?\n    tty.setRawMode(true);\n    this.enabled = true;\n\n    // Cursor position on the line.\n    this.cursor = 0;\n\n    this.history = [];\n    this.historyIndex = -1;\n\n    var winSize = output.getWindowSize();\n    exports.columns = winSize[1];\n\n    if (process.listeners('SIGWINCH').length === 0) {\n      process.on('SIGWINCH', function() {\n        var winSize = output.getWindowSize();\n        exports.columns = winSize[1];\n      });\n    }\n  }\n}\n\ninherits(Interface, EventEmitter);\n\nInterface.prototype.__defineGetter__('columns', function() {\n  return exports.columns;\n});\n\nInterface.prototype.setPrompt = function(prompt, length) {\n  this._prompt = prompt;\n  if (length) {\n    this._promptLength = length;\n  } else {\n    var lines = prompt.split(/[\\r\\n]/);\n    var lastLine = lines[lines.length - 1];\n    this._promptLength = Buffer.byteLength(lastLine);\n  }\n};\n\n\nInterface.prototype.prompt = function() {\n  if (this.enabled) {\n    this.cursor = 0;\n    this._refreshLine();\n  } else {\n    this.output.write(this._prompt);\n  }\n};\n\n\nInterface.prototype.question = function(query, cb) {\n  if (cb) {\n    this.resume();\n    if (this._questionCallback) {\n      this.output.write('\\n');\n      this.prompt();\n    } else {\n      this._oldPrompt = this._prompt;\n      this.setPrompt(query);\n      this._questionCallback = cb;\n      this.output.write('\\n');\n      this.prompt();\n    }\n  }\n};\n\n\nInterface.prototype._onLine = function(line) {\n  if (this._questionCallback) {\n    var cb = this._questionCallback;\n    this._questionCallback = null;\n    this.setPrompt(this._oldPrompt);\n    cb(line);\n  } else {\n    this.emit('line', line);\n  }\n};\n\n\nInterface.prototype._addHistory = function() {\n  if (this.line.length === 0) return '';\n\n  this.history.unshift(this.line);\n  this.line = '';\n  this.historyIndex = -1;\n\n  this.cursor = 0;\n\n  // Only store so many\n  if (this.history.length > kHistorySize) this.history.pop();\n\n  return this.history[0];\n};\n\n\nInterface.prototype._refreshLine = function() {\n  if (this._closed) return;\n\n  // Cursor to left edge.\n  this.output.cursorTo(0);\n\n  // Write the prompt and the current buffer content.\n  this.output.write(this._prompt);\n  this.output.write(this.line);\n\n  // Erase to right.\n  this.output.clearLine(1);\n\n  // Move cursor to original position.\n  this.output.cursorTo(this._promptLength + this.cursor);\n};\n\n\nInterface.prototype.close = function(d) {\n  if (this._closing) return;\n  this._closing = true;\n  if (this.enabled) {\n    tty.setRawMode(false);\n  }\n  this.emit('close');\n  this._closed = true;\n};\n\n\nInterface.prototype.pause = function() {\n  if (this.enabled) {\n    tty.setRawMode(false);\n  }\n};\n\n\nInterface.prototype.resume = function() {\n  if (this.enabled) {\n    tty.setRawMode(true);\n  }\n};\n\n\nInterface.prototype.write = function(d, key) {\n  if (this._closed) return;\n  this.enabled ? this._ttyWrite(d, key) : this._normalWrite(d, key);\n};\n\n\nInterface.prototype._normalWrite = function(b) {\n  // Very simple implementation right now. Should try to break on\n  // new lines.\n  if (b !== undefined)\n    this._onLine(b.toString());\n};\n\nInterface.prototype._insertString = function(c) {\n  //BUG: Problem when adding tabs with following content.\n  //     Perhaps the bug is in _refreshLine(). Not sure.\n  //     A hack would be to insert spaces instead of literal '\\t'.\n  if (this.cursor < this.line.length) {\n    var beg = this.line.slice(0, this.cursor);\n    var end = this.line.slice(this.cursor, this.line.length);\n    this.line = beg + c + end;\n    this.cursor += c.length;\n    this._refreshLine();\n  } else {\n    this.line += c;\n    this.cursor += c.length;\n    this.output.write(c);\n  }\n};\n\nInterface.prototype._tabComplete = function() {\n  var self = this;\n\n  self.pause();\n  self.completer(self.line.slice(0, self.cursor), function(err, rv) {\n    self.resume();\n\n    if (err) {\n      // XXX Log it somewhere?\n      return;\n    }\n\n    var completions = rv[0],\n        completeOn = rv[1];  // the text that was completed\n    if (completions && completions.length) {\n      // Apply/show completions.\n      if (completions.length === 1) {\n        self._insertString(completions[0].slice(completeOn.length));\n      } else {\n        self.output.write('\\r\\n');\n        var width = completions.reduce(function(a, b) {\n          return a.length > b.length ? a : b;\n        }).length + 2;  // 2 space padding\n        var maxColumns = Math.floor(self.columns / width) || 1;\n\n        function handleGroup(group) {\n          if (group.length == 0) {\n            return;\n          }\n          var minRows = Math.ceil(group.length / maxColumns);\n          for (var row = 0; row < minRows; row++) {\n            for (var col = 0; col < maxColumns; col++) {\n              var idx = row * maxColumns + col;\n              if (idx >= group.length) {\n                break;\n              }\n              var item = group[idx];\n              self.output.write(item);\n              if (col < maxColumns - 1) {\n                for (var s = 0, itemLen = item.length; s < width - itemLen;\n                     s++) {\n                  self.output.write(' ');\n                }\n              }\n            }\n            self.output.write('\\r\\n');\n          }\n          self.output.write('\\r\\n');\n        }\n\n        var group = [], c;\n        for (var i = 0, compLen = completions.length; i < compLen; i++) {\n          c = completions[i];\n          if (c === '') {\n            handleGroup(group);\n            group = [];\n          } else {\n            group.push(c);\n          }\n        }\n        handleGroup(group);\n\n        // If there is a common prefix to all matches, then apply that\n        // portion.\n        var f = completions.filter(function(e) { if (e) return e; });\n        var prefix = commonPrefix(f);\n        if (prefix.length > completeOn.length) {\n          self._insertString(prefix.slice(completeOn.length));\n        }\n\n      }\n      self._refreshLine();\n    }\n  });\n};\n\n\nfunction commonPrefix(strings) {\n  if (!strings || strings.length == 0) {\n    return '';\n  }\n  var sorted = strings.slice().sort();\n  var min = sorted[0];\n  var max = sorted[sorted.length - 1];\n  for (var i = 0, len = min.length; i < len; i++) {\n    if (min[i] != max[i]) {\n      return min.slice(0, i);\n    }\n  }\n  return min;\n}\n\n\nInterface.prototype._wordLeft = function() {\n  if (this.cursor > 0) {\n    var leading = this.line.slice(0, this.cursor);\n    var match = leading.match(/([^\\w\\s]+|\\w+|)\\s*$/);\n    this.cursor -= match[0].length;\n    this._refreshLine();\n  }\n};\n\n\nInterface.prototype._wordRight = function() {\n  if (this.cursor < this.line.length) {\n    var trailing = this.line.slice(this.cursor);\n    var match = trailing.match(/^(\\s+|\\W+|\\w+)\\s*/);\n    this.cursor += match[0].length;\n    this._refreshLine();\n  }\n};\n\n\nInterface.prototype._deleteLeft = function() {\n  if (this.cursor > 0 && this.line.length > 0) {\n    this.line = this.line.slice(0, this.cursor - 1) +\n                this.line.slice(this.cursor, this.line.length);\n\n    this.cursor--;\n    this._refreshLine();\n  }\n};\n\n\nInterface.prototype._deleteRight = function() {\n  this.line = this.line.slice(0, this.cursor) +\n              this.line.slice(this.cursor + 1, this.line.length);\n  this._refreshLine();\n};\n\n\nInterface.prototype._deleteWordLeft = function() {\n  if (this.cursor > 0) {\n    var leading = this.line.slice(0, this.cursor);\n    var match = leading.match(/([^\\w\\s]+|\\w+|)\\s*$/);\n    leading = leading.slice(0, leading.length - match[0].length);\n    this.line = leading + this.line.slice(this.cursor, this.line.length);\n    this.cursor = leading.length;\n    this._refreshLine();\n  }\n};\n\n\nInterface.prototype._deleteWordRight = function() {\n  if (this.cursor < this.line.length) {\n    var trailing = this.line.slice(this.cursor);\n    var match = trailing.match(/^(\\s+|\\W+|\\w+)\\s*/);\n    this.line = this.line.slice(0, this.cursor) +\n                trailing.slice(match[0].length);\n    this._refreshLine();\n  }\n};\n\n\nInterface.prototype._deleteLineLeft = function() {\n  this.line = this.line.slice(this.cursor);\n  this.cursor = 0;\n  this._refreshLine();\n};\n\n\nInterface.prototype._deleteLineRight = function() {\n  this.line = this.line.slice(0, this.cursor);\n  this._refreshLine();\n};\n\n\nInterface.prototype._line = function() {\n  var line = this._addHistory();\n  this.output.write('\\r\\n');\n  this._onLine(line);\n};\n\n\nInterface.prototype._historyNext = function() {\n  if (this.historyIndex > 0) {\n    this.historyIndex--;\n    this.line = this.history[this.historyIndex];\n    this.cursor = this.line.length; // set cursor to end of line.\n    this._refreshLine();\n\n  } else if (this.historyIndex === 0) {\n    this.historyIndex = -1;\n    this.cursor = 0;\n    this.line = '';\n    this._refreshLine();\n  }\n};\n\n\nInterface.prototype._historyPrev = function() {\n  if (this.historyIndex + 1 < this.history.length) {\n    this.historyIndex++;\n    this.line = this.history[this.historyIndex];\n    this.cursor = this.line.length; // set cursor to end of line.\n\n    this._refreshLine();\n  }\n};\n\n\nInterface.prototype._attemptClose = function() {\n  if (this.listeners('attemptClose').length) {\n    // User is to call interface.close() manually.\n    this.emit('attemptClose');\n  } else {\n    this.close();\n  }\n};\n\n\n// handle a write from the tty\nInterface.prototype._ttyWrite = function(s, key) {\n  var next_word, next_non_word, previous_word, previous_non_word;\n  key = key || {};\n\n  if (key.ctrl && key.shift) {\n    /* Control and shift pressed */\n    switch (key.name) {\n      case 'backspace':\n        this._deleteLineLeft();\n        break;\n\n      case 'delete':\n        this._deleteLineRight();\n        break;\n    }\n\n  } else if (key.ctrl) {\n    /* Control key pressed */\n\n    switch (key.name) {\n      case 'c':\n        if (this.listeners('SIGINT').length) {\n          this.emit('SIGINT');\n        } else {\n          // default behavior, end the readline\n          this._attemptClose();\n        }\n        break;\n\n      case 'h': // delete left\n        this._deleteLeft();\n        break;\n\n      case 'd': // delete right or EOF\n        if (this.cursor === 0 && this.line.length === 0) {\n          this._attemptClose();\n        } else if (this.cursor < this.line.length) {\n          this._deleteRight();\n        }\n        break;\n\n      case 'u': // delete the whole line\n        this.cursor = 0;\n        this.line = '';\n        this._refreshLine();\n        break;\n\n      case 'k': // delete from current to end of line\n        this._deleteLineRight();\n        break;\n\n      case 'a': // go to the start of the line\n        this.cursor = 0;\n        this._refreshLine();\n        break;\n\n      case 'e': // go to the end of the line\n        this.cursor = this.line.length;\n        this._refreshLine();\n        break;\n\n      case 'b': // back one character\n        if (this.cursor > 0) {\n          this.cursor--;\n          this._refreshLine();\n        }\n        break;\n\n      case 'f': // forward one character\n        if (this.cursor != this.line.length) {\n          this.cursor++;\n          this._refreshLine();\n        }\n        break;\n\n      case 'n': // next history item\n        this._historyNext();\n        break;\n\n      case 'p': // previous history item\n        this._historyPrev();\n        break;\n\n      case 'z':\n        process.kill(process.pid, 'SIGTSTP');\n        return;\n\n      case 'w': // delete backwards to a word boundary\n      case 'backspace':\n        this._deleteWordLeft();\n        break;\n\n      case 'delete': // delete forward to a word boundary\n        this._deleteWordRight();\n        break;\n\n      case 'backspace':\n        this._deleteWordLeft();\n        break;\n\n      case 'left':\n        this._wordLeft();\n        break;\n\n      case 'right':\n        this._wordRight();\n    }\n\n  } else if (key.meta) {\n    /* Meta key pressed */\n\n    switch (key.name) {\n      case 'b': // backward word\n        this._wordLeft();\n        break;\n\n      case 'f': // forward word\n        this._wordRight();\n        break;\n\n      case 'd': // delete forward word\n      case 'delete':\n        this._deleteWordRight();\n        break;\n\n      case 'backspace': // delete backwards to a word boundary\n        this._deleteWordLeft();\n        break;\n    }\n\n  } else {\n    /* No modifier keys used */\n\n    switch (key.name) {\n      case 'enter':\n        this._line();\n        break;\n\n      case 'backspace':\n        this._deleteLeft();\n        break;\n\n      case 'delete':\n        this._deleteRight();\n        break;\n\n      case 'tab': // tab completion\n        this._tabComplete();\n        break;\n\n      case 'left':\n        if (this.cursor > 0) {\n          this.cursor--;\n          this.output.moveCursor(-1, 0);\n        }\n        break;\n\n      case 'right':\n        if (this.cursor != this.line.length) {\n          this.cursor++;\n          this.output.moveCursor(1, 0);\n        }\n        break;\n\n      case 'home':\n        this.cursor = 0;\n        this._refreshLine();\n        break;\n\n      case 'end':\n        this.cursor = this.line.length;\n        this._refreshLine();\n        break;\n\n      case 'up':\n        this._historyPrev();\n        break;\n\n      case 'down':\n        this._historyNext();\n        break;\n\n      default:\n        if (Buffer.isBuffer(s))\n          s = s.toString('utf-8');\n\n        if (s) {\n          var lines = s.split(/\\r\\n|\\n|\\r/);\n          for (var i = 0, len = lines.length; i < len; i++) {\n            if (i > 0) {\n              this._line();\n            }\n            this._insertString(lines[i]);\n          }\n        }\n    }\n  }\n};\n\n\nexports.Interface = Interface;\n})", "gear:readline")(exports, require, module);
}
static NativeModule _module_readline("readline", _setup_readline);
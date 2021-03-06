#line 1 "src/modules/fs.gear"

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

/** \file src/modules/fs.cc converted from src/modules/fs.gear */

#line 25 "src/modules/fs.gear"


#include <cstring>
#include <errno.h>
#include <UvCommon.h>

#include "buffer.h"

// hack alert! copy of ErrnoException in node.cc, tuned for uv errors
Value FsError(int errorNo, String syscall, String msg, String path) {
    uv_err_t uvErr;
    std::memset(&uvErr, 0, sizeof(uv_err_t));
    uvErr.code = uv_err_code(errorNo);
    
    if(!msg.length())
        msg = uv_err_name(uvErr);

    String errCode = uv_strerror(uvErr), errString = errCode + ", " + msg;

    var err = Error(path.length() ? errString + " '" + path + "'" : errString);

    // TODO errno should probably go
    err["errno"] = errorNo;
    err["code"] = errCode;
    if(path.length())
        err["path"] = path;
    if(syscall.length())
        err["syscall"] = syscall;
    return err;
}

// Converts an unixtime to V8 Date
#define NODE_UNIXTIME_V8(t) v8::Date::New(1000*double(t))
#define NODE_V8_UNIXTIME(v) (double(v)/1000.0)

// Use different stat structs & calls on windows and posix;
// on windows, _stati64 is utf-8 and big file aware.
#if __POSIX__
# define NODE_STAT        stat
# define NODE_FSTAT       fstat
# define NODE_STAT_STRUCT struct stat
#else // __MINGW32__
# define NODE_STAT        _stati64
# define NODE_FSTAT       _fstati64
# define NODE_STAT_STRUCT struct _stati64
#endif

static var statsCtor;

Value makeStats(NODE_STAT_STRUCT *s) {
    var stats = statsCtor.newInstance();
    
    // ID of device containing file
    stats["dev"] = s->st_dev;
    
    // inode number
    stats["ino"] = s->st_ino;
    
    // protection
    stats["mode"] = s->st_mode;
    
    // number of hard links
    stats["nlink"] = s->st_nlink;
    
    // user ID of owner
    stats["uid"] = s->st_uid;
    
    // group ID of owner
    stats["gid"] = s->st_gid;
    
    // device ID (if special file)
    stats["rdev"] = s->st_rdev;
    
    // total size, in bytes
    stats["size"] = s->st_size;
    
#ifdef __POSIX__
    // blocksize for filesystem I/O
    stats["blksize"] = s->st_blksize;
    
    // number of blocks allocated
    stats["blocks"] = s->st_blocks;
#endif
    
    // time of last access
    stats["atime"] = NODE_UNIXTIME_V8(s->st_atime);
    
    // time of last modification
    stats["mtime"] = NODE_UNIXTIME_V8(s->st_mtime);
    
    // time of last status change
    stats["ctime"] = NODE_UNIXTIME_V8(s->st_ctime);
    
    return stats;
}

#define FS_CALL(func, cb, path, ...)                                        \
if(args[cb]->IsFunction())                                              \
    return undefined;                                                   \
uv_fs_t req;                                                            \
int result = uv_fs_##func(uv_default_loop(), &req, __VA_ARGS__, NULL);  \
if(result < 0)                                                          \
    return Throw(FsError(uv_last_error(uv_default_loop()).code, #func, "", path));
#define FS_CALL_RINT(...) FS_CALL(__VA_ARGS__);return Integer(result);
#define FS_CALL_RSTAT(...) FS_CALL(__VA_ARGS__);return makeStats(reinterpret_cast<NODE_STAT_STRUCT*>(req.ptr));
#define GET_OFFSET(a) ((a)->IsNumber() ? (a)->IntegerValue() : -1) //FIXME optArgs

static v8::Handle<v8::Value> _fs_binding_Stats_Stats(const v8::Arguments &args) {
    Value This(args.This());
    #line 136 "src/modules/fs.gear"
    
    return undefined;
}

static v8::Handle<v8::Value> _fs_binding_close(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 137 "src/modules/fs.gear"
        Value fd(args[0]);
        FS_CALL(close, 0, "", fd);
        return undefined;
    }
    THROW_ERROR("Invalid call to fs.binding.close");
}

static v8::Handle<v8::Value> _fs_binding_open(const v8::Arguments &args) {
    if(args.Length() >= 3) {
        #line 140 "src/modules/fs.gear"
        Value path(args[0]), flags(args[1]), mode(args[2]);
        FS_CALL_RINT(open, 3, path, path.to<String>(), flags, mode);
        return undefined;
    }
    THROW_ERROR("Invalid call to fs.binding.open");
}

static v8::Handle<v8::Value> _fs_binding_read(const v8::Arguments &args) {
    if(args.Length() >= 4) {
        #line 143 "src/modules/fs.gear"
        Value fd(args[0]), buffer(args[1]), _offset(args[2]), _length(args[3]);
        if(!buffer.is<Buffer>())
            THROW_TYPE_ERROR("Second argument needs to be a buffer");
        
        size_t bufferLength = Buffer::length(buffer), offset = _offset;
        if(offset >= bufferLength)
            THROW_ERROR("Offset is out of bounds");
        
        size_t length = _length;
        if(offset + length > bufferLength)
            THROW_ERROR("Length extends beyond buffer");
        
        FS_CALL_RINT(read, 5, "", fd, Buffer::data(buffer) + offset, length, GET_OFFSET(args[4]));
        return undefined;
    }
    THROW_ERROR("Invalid call to fs.binding.read");
}

static v8::Handle<v8::Value> _fs_binding_write(const v8::Arguments &args) {
    if(args.Length() >= 4) {
        #line 158 "src/modules/fs.gear"
        Value fd(args[0]), buffer(args[1]), _offset(args[2]), _length(args[3]);
        if(!buffer.is<Buffer>())
            THROW_TYPE_ERROR("Second argument needs to be a buffer");
        
        size_t bufferLength = Buffer::length(buffer), offset = _offset;
        if(offset >= bufferLength)
            THROW_ERROR("Offset is out of bounds");
        
        size_t length = _length;
        if(offset + length > bufferLength)
            THROW_ERROR("Length extends beyond buffer");
        
        FS_CALL_RINT(write, 5, "", fd, Buffer::data(buffer) + offset, length, GET_OFFSET(args[4]));
        return undefined;
    }
    THROW_ERROR("Invalid call to fs.binding.write");
}

static v8::Handle<v8::Value> _fs_binding_stat(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 173 "src/modules/fs.gear"
        Value path(args[0]);
        FS_CALL_RSTAT(stat, 1, path, path.to<String>());
        return undefined;
    }
    THROW_ERROR("Invalid call to fs.binding.stat");
}

static v8::Handle<v8::Value> _fs_binding_lstat(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 175 "src/modules/fs.gear"
        Value path(args[0]);
        FS_CALL_RSTAT(lstat, 1, path, path.to<String>());
        return undefined;
    }
    THROW_ERROR("Invalid call to fs.binding.lstat");
}

static v8::Handle<v8::Value> _fs_binding_fstat(const v8::Arguments &args) {
    if(args.Length() >= 1) {
        #line 177 "src/modules/fs.gear"
        Value fd(args[0]);
        FS_CALL_RSTAT(fstat, 1, "", fd);
        return undefined;
    }
    THROW_ERROR("Invalid call to fs.binding.fstat");
}


#line 239 "src/modules/fs.cc"
static void _setup_fs(Value exports, Value require, Value module) {
    var binding = Object();
    v8::Handle<v8::FunctionTemplate> _fs_binding_Stats = v8::FunctionTemplate::New(_fs_binding_Stats_Stats);
    _fs_binding_Stats->SetClassName(String("Stats"));
    binding["Stats"] = _fs_binding_Stats->GetFunction();
    binding["close"] = Function(_fs_binding_close, "close");
    binding["open"] = Function(_fs_binding_open, "open");
    binding["read"] = Function(_fs_binding_read, "read");
    binding["write"] = Function(_fs_binding_write, "write");
    binding["stat"] = Function(_fs_binding_stat, "stat");
    binding["lstat"] = Function(_fs_binding_lstat, "lstat");
    binding["fstat"] = Function(_fs_binding_fstat, "fstat");
    #line 181 "src/modules/fs.gear"

        statsCtor = binding["Stats"];
        Context::getCurrent()->runScript("(function(exports, require, module, binding){\n//BEGIN lib/fs.js\n// Maintainers, keep in mind that octal literals are not allowed\n// in strict mode. Use the decimal value and add a comment with\n// the octal value. Example:\n//\n//   var mode = 438; /* mode=0666 */\n\nvar util = require('util');\nvar pathModule = require('path');\n\n//BEGIN #gearbox\n//var binding = process.binding('fs');\n//var constants = process.binding('constants');\nvar constants = require('constants');\n//END #gearbox\nvar fs = exports;\nvar Stream = require('stream').Stream;\nvar EventEmitter = require('events').EventEmitter;\n\nvar kMinPoolSpace = 128;\nvar kPoolSize = 40 * 1024;\n\nfs.Stats = binding.Stats;\n\nfs.Stats.prototype._checkModeProperty = function(property) {\n  return ((this.mode & constants.S_IFMT) === property);\n};\n\nfs.Stats.prototype.isDirectory = function() {\n  return this._checkModeProperty(constants.S_IFDIR);\n};\n\nfs.Stats.prototype.isFile = function() {\n  return this._checkModeProperty(constants.S_IFREG);\n};\n\nfs.Stats.prototype.isBlockDevice = function() {\n  return this._checkModeProperty(constants.S_IFBLK);\n};\n\nfs.Stats.prototype.isCharacterDevice = function() {\n  return this._checkModeProperty(constants.S_IFCHR);\n};\n\nfs.Stats.prototype.isSymbolicLink = function() {\n  return this._checkModeProperty(constants.S_IFLNK);\n};\n\nfs.Stats.prototype.isFIFO = function() {\n  return this._checkModeProperty(constants.S_IFIFO);\n};\n\nfs.Stats.prototype.isSocket = function() {\n  return this._checkModeProperty(constants.S_IFSOCK);\n};\n\nfs.readFile = function(path, encoding_) {\n  var encoding = typeof(encoding_) === 'string' ? encoding_ : null;\n  var callback = arguments[arguments.length - 1];\n  if (typeof(callback) !== 'function') callback = noop;\n  var readStream = fs.createReadStream(path);\n  var buffers = [];\n  var nread = 0;\n\n  readStream.on('data', function(chunk) {\n    buffers.push(chunk);\n    nread += chunk.length;\n  });\n\n  readStream.on('error', function(er) {\n    callback(er);\n    readStream.destroy();\n  });\n\n  readStream.on('end', function() {\n    // copy all the buffers into one\n    var buffer;\n    switch (buffers.length) {\n      case 0: buffer = new Buffer(0); break;\n      case 1: buffer = buffers[0]; break;\n      default: // concat together\n        buffer = new Buffer(nread);\n        var n = 0;\n        buffers.forEach(function(b) {\n          var l = b.length;\n          b.copy(buffer, n, 0, l);\n          n += l;\n        });\n        break;\n    }\n    if (encoding) {\n      try {\n        buffer = buffer.toString(encoding);\n      } catch (er) {\n        return callback(er);\n      }\n    }\n    callback(null, buffer);\n  });\n};\n\nfs.readFileSync = function(path, encoding) {\n  var fd = fs.openSync(path, constants.O_RDONLY, 438 /*=0666*/);\n  var buffer = new Buffer(4048);\n  var buffers = [];\n  var nread = 0;\n  var lastRead = 0;\n\n  do {\n    if (lastRead) {\n      buffer._bytesRead = lastRead;\n      nread += lastRead;\n      buffers.push(buffer);\n    }\n    var buffer = new Buffer(4048);\n    lastRead = fs.readSync(fd, buffer, 0, buffer.length, null);\n  } while (lastRead > 0);\n\n  fs.closeSync(fd);\n\n  if (buffers.length > 1) {\n    var offset = 0;\n    var i;\n    buffer = new Buffer(nread);\n    buffers.forEach(function(i) {\n      if (!i._bytesRead) return;\n      i.copy(buffer, offset, 0, i._bytesRead);\n      offset += i._bytesRead;\n    });\n  } else if (buffers.length) {\n    // buffers has exactly 1 (possibly zero length) buffer, so this should\n    // be a shortcut\n    buffer = buffers[0].slice(0, buffers[0]._bytesRead);\n  } else {\n    buffer = new Buffer(0);\n  }\n\n  if (encoding) buffer = buffer.toString(encoding);\n  return buffer;\n};\n\n\n// Used by binding.open and friends\nfunction stringToFlags(flag) {\n  // Only mess with strings\n  if (typeof flag !== 'string') {\n    return flag;\n  }\n  switch (flag) {\n    case 'r':\n      return constants.O_RDONLY;\n\n    case 'r+':\n      return constants.O_RDWR;\n\n    case 'w':\n      return constants.O_CREAT | constants.O_TRUNC | constants.O_WRONLY;\n\n    case 'w+':\n      return constants.O_CREAT | constants.O_TRUNC | constants.O_RDWR;\n\n    case 'a':\n      return constants.O_APPEND | constants.O_CREAT | constants.O_WRONLY;\n\n    case 'a+':\n      return constants.O_APPEND | constants.O_CREAT | constants.O_RDWR;\n\n    default:\n      throw new Error('Unknown file open flag: ' + flag);\n  }\n}\n\nfunction noop() {}\n\n// Yes, the follow could be easily DRYed up but I provide the explicit\n// list to make the arguments clear.\n\nfs.close = function(fd, callback) {\n  binding.close(fd, callback || noop);\n};\n\nfs.closeSync = function(fd) {\n  return binding.close(fd);\n};\n\nfunction modeNum(m, def) {\n  switch (typeof m) {\n    case 'number': return m;\n    case 'string': return parseInt(m, 8);\n    default:\n      if (def) {\n        return modeNum(def);\n      } else {\n        return undefined;\n      }\n  }\n}\n\nfs.open = function(path, flags, mode, callback) {\n  callback = arguments[arguments.length - 1];\n  if (typeof(callback) !== 'function') {\n    callback = noop;\n  }\n\n  mode = modeNum(mode, 438 /*=0666*/);\n\n  binding.open(pathModule._makeLong(path), stringToFlags(flags), mode,\n    callback);\n};\n\nfs.openSync = function(path, flags, mode) {\n  mode = modeNum(mode, 438 /*=0666*/);\n  return binding.open(pathModule._makeLong(path), stringToFlags(flags), mode);\n};\n\nfs.read = function(fd, buffer, offset, length, position, callback) {\n  if (!Buffer.isBuffer(buffer)) {\n    // legacy string interface (fd, length, position, encoding, callback)\n    var cb = arguments[4],\n        encoding = arguments[3];\n    position = arguments[2];\n    length = arguments[1];\n    buffer = new Buffer(length);\n    offset = 0;\n\n    callback = function(err, bytesRead) {\n      if (!cb) return;\n\n      var str = (bytesRead > 0) ? buffer.toString(encoding, 0, bytesRead) : '';\n\n      (cb)(err, str, bytesRead);\n    };\n  }\n\n  function wrapper(err, bytesRead) {\n    // Retain a reference to buffer so that it can't be GC'ed too soon.\n    callback && callback(err, bytesRead || 0, buffer);\n  }\n\n  binding.read(fd, buffer, offset, length, position, wrapper);\n};\n\nfs.readSync = function(fd, buffer, offset, length, position) {\n  var legacy = false;\n  if (!Buffer.isBuffer(buffer)) {\n    // legacy string interface (fd, length, position, encoding, callback)\n    legacy = true;\n    var encoding = arguments[3];\n    position = arguments[2];\n    length = arguments[1];\n    buffer = new Buffer(length);\n\n    offset = 0;\n  }\n\n  var r = binding.read(fd, buffer, offset, length, position);\n  if (!legacy) {\n    return r;\n  }\n\n  var str = (r > 0) ? buffer.toString(encoding, 0, r) : '';\n  return [str, r];\n};\n\nfs.write = function(fd, buffer, offset, length, position, callback) {\n  if (!Buffer.isBuffer(buffer)) {\n    // legacy string interface (fd, data, position, encoding, callback)\n    callback = arguments[4];\n    position = arguments[2];\n\n    buffer = new Buffer('' + arguments[1], arguments[3]);\n    offset = 0;\n    length = buffer.length;\n  }\n\n  if (!length) {\n    if (typeof callback == 'function') {\n      process.nextTick(function() {\n        callback(undefined, 0);\n      });\n    }\n    return;\n  }\n\n  function wrapper(err, written) {\n    // Retain a reference to buffer so that it can't be GC'ed too soon.\n    callback && callback(err, written || 0, buffer);\n  }\n\n  binding.write(fd, buffer, offset, length, position, wrapper);\n};\n\nfs.writeSync = function(fd, buffer, offset, length, position) {\n  if (!Buffer.isBuffer(buffer)) {\n    // legacy string interface (fd, data, position, encoding)\n    position = arguments[2];\n\n    buffer = new Buffer('' + arguments[1], arguments[3]);\n    offset = 0;\n    length = buffer.length;\n  }\n  if (!length) return 0;\n\n  return binding.write(fd, buffer, offset, length, position);\n};\n\nfs.rename = function(oldPath, newPath, callback) {\n  binding.rename(pathModule._makeLong(oldPath), pathModule._makeLong(newPath),\n    callback || noop);\n};\n\nfs.renameSync = function(oldPath, newPath) {\n  return binding.rename(pathModule._makeLong(oldPath),\n    pathModule._makeLong(newPath));\n};\n\nfs.truncate = function(fd, len, callback) {\n  binding.truncate(fd, len, callback || noop);\n};\n\nfs.truncateSync = function(fd, len) {\n  return binding.truncate(fd, len);\n};\n\nfs.rmdir = function(path, callback) {\n  binding.rmdir(pathModule._makeLong(path), callback || noop);\n};\n\nfs.rmdirSync = function(path) {\n  return binding.rmdir(pathModule._makeLong(path));\n};\n\nfs.fdatasync = function(fd, callback) {\n  binding.fdatasync(fd, callback || noop);\n};\n\nfs.fdatasyncSync = function(fd) {\n  return binding.fdatasync(fd);\n};\n\nfs.fsync = function(fd, callback) {\n  binding.fsync(fd, callback || noop);\n};\n\nfs.fsyncSync = function(fd) {\n  return binding.fsync(fd);\n};\n\nfs.mkdir = function(path, mode, callback) {\n  if (typeof mode === 'function') callback = mode;\n  binding.mkdir(pathModule._makeLong(path), modeNum(mode, 511 /*=0777*/),\n    callback || noop);\n};\n\nfs.mkdirSync = function(path, mode) {\n  return binding.mkdir(pathModule._makeLong(path),\n    modeNum(mode, 511 /*=0777*/));\n};\n\nfs.sendfile = function(outFd, inFd, inOffset, length, callback) {\n  binding.sendfile(outFd, inFd, inOffset, length, callback || noop);\n};\n\nfs.sendfileSync = function(outFd, inFd, inOffset, length) {\n  return binding.sendfile(outFd, inFd, inOffset, length);\n};\n\nfs.readdir = function(path, callback) {\n  binding.readdir(pathModule._makeLong(path), callback || noop);\n};\n\nfs.readdirSync = function(path) {\n  return binding.readdir(pathModule._makeLong(path));\n};\n\nfs.fstat = function(fd, callback) {\n  binding.fstat(fd, callback || noop);\n};\n\nfs.lstat = function(path, callback) {\n  binding.lstat(pathModule._makeLong(path), callback || noop);\n};\n\nfs.stat = function(path, callback) {\n  binding.stat(pathModule._makeLong(path), callback || noop);\n};\n\nfs.fstatSync = function(fd) {\n  return binding.fstat(fd);\n};\n\nfs.lstatSync = function(path) {\n  return binding.lstat(pathModule._makeLong(path));\n};\n\nfs.statSync = function(path) {\n  return binding.stat(pathModule._makeLong(path));\n};\n\nfs.readlink = function(path, callback) {\n  binding.readlink(pathModule._makeLong(path), callback || noop);\n};\n\nfs.readlinkSync = function(path) {\n  return binding.readlink(pathModule._makeLong(path));\n};\n\nfs.symlink = function(destination, path, mode_, callback) {\n  var mode = (typeof(mode_) == 'string' ? mode_ : null);\n  var callback_ = arguments[arguments.length - 1];\n  callback = (typeof(callback_) == 'function' ? callback_ : null);\n  binding.symlink(pathModule._makeLong(destination),\n    pathModule._makeLong(path), mode, callback);\n};\n\nfs.symlinkSync = function(destination, path, mode) {\n  return binding.symlink(pathModule._makeLong(destination),\n    pathModule._makeLong(path), mode);\n};\n\nfs.link = function(srcpath, dstpath, callback) {\n  binding.link(pathModule._makeLong(srcpath), pathModule._makeLong(dstpath),\n    callback || noop);\n};\n\nfs.linkSync = function(srcpath, dstpath) {\n  return binding.link(pathModule._makeLong(srcpath),\n    pathModule._makeLong(dstpath));\n};\n\nfs.unlink = function(path, callback) {\n  binding.unlink(pathModule._makeLong(path), callback || noop);\n};\n\nfs.unlinkSync = function(path) {\n  return binding.unlink(pathModule._makeLong(path));\n};\n\nfs.fchmod = function(fd, mode, callback) {\n  binding.fchmod(fd, modeNum(mode), callback || noop);\n};\n\nfs.fchmodSync = function(fd, mode) {\n  return binding.fchmod(fd, modeNum(mode));\n};\n\nif (constants.hasOwnProperty('O_SYMLINK')) {\n  fs.lchmod = function(path, mode, callback) {\n    callback = callback || noop;\n    fs.open(path, constants.O_WRONLY | constants.O_SYMLINK, function(err, fd) {\n      if (err) {\n        callback(err);\n        return;\n      }\n      // prefer to return the chmod error, if one occurs,\n      // but still try to close, and report closing errors if they occur.\n      fs.fchmod(fd, mode, function(err) {\n        fs.close(fd, function(err2) {\n          callback(err || err2);\n        });\n      });\n    });\n  };\n\n  fs.lchmodSync = function(path, mode) {\n    var fd = fs.openSync(path, constants.O_WRONLY | constants.O_SYMLINK);\n\n    // prefer to return the chmod error, if one occurs,\n    // but still try to close, and report closing errors if they occur.\n    var err, err2;\n    try {\n      var ret = fs.fchmodSync(fd, mode);\n    } catch (er) {\n      err = er;\n    }\n    try {\n      fs.closeSync(fd);\n    } catch (er) {\n      err2 = er;\n    }\n    if (err || err2) throw (err || err2);\n    return ret;\n  };\n}\n\n\nfs.chmod = function(path, mode, callback) {\n  binding.chmod(pathModule._makeLong(path), modeNum(mode), callback || noop);\n};\n\nfs.chmodSync = function(path, mode) {\n  return binding.chmod(pathModule._makeLong(path), modeNum(mode));\n};\n\nif (constants.hasOwnProperty('O_SYMLINK')) {\n  fs.lchown = function(path, uid, gid, callback) {\n    callback = callback || noop;\n    fs.open(path, constants.O_WRONLY | constants.O_SYMLINK, function(err, fd) {\n      if (err) {\n        callback(err);\n        return;\n      }\n      fs.fchown(fd, uid, gid, callback);\n    });\n  };\n\n  fs.lchownSync = function(path, uid, gid) {\n    var fd = fs.openSync(path, constants.O_WRONLY | constants.O_SYMLINK);\n    return fs.fchownSync(fd, uid, gid);\n  };\n}\n\nfs.fchown = function(fd, uid, gid, callback) {\n  binding.fchown(fd, uid, gid, callback || noop);\n};\n\nfs.fchownSync = function(fd, uid, gid) {\n  return binding.fchown(fd, uid, gid);\n};\n\nfs.chown = function(path, uid, gid, callback) {\n  binding.chown(pathModule._makeLong(path), uid, gid, callback || noop);\n};\n\nfs.chownSync = function(path, uid, gid) {\n  return binding.chown(pathModule._makeLong(path), uid, gid);\n};\n\n// converts Date or number to a fractional UNIX timestamp\nfunction toUnixTimestamp(time) {\n  if (typeof time == 'number') {\n    return time;\n  }\n  if (time instanceof Date) {\n    // convert to 123.456 UNIX timestamp\n    return time.getTime() / 1000;\n  }\n  throw new Error('Cannot parse time: ' + time);\n}\n\n// exported for unit tests, not for public consumption\nfs._toUnixTimestamp = toUnixTimestamp;\n\nfs.utimes = function(path, atime, mtime, callback) {\n  atime = toUnixTimestamp(atime);\n  mtime = toUnixTimestamp(mtime);\n  binding.utimes(pathModule._makeLong(path), atime, mtime, callback || noop);\n};\n\nfs.utimesSync = function(path, atime, mtime) {\n  atime = toUnixTimestamp(atime);\n  mtime = toUnixTimestamp(mtime);\n  binding.utimes(pathModule._makeLong(path), atime, mtime);\n};\n\nfs.futimes = function(fd, atime, mtime, callback) {\n  atime = toUnixTimestamp(atime);\n  mtime = toUnixTimestamp(mtime);\n  binding.futimes(fd, atime, mtime, callback || noop);\n};\n\nfs.futimesSync = function(fd, atime, mtime) {\n  atime = toUnixTimestamp(atime);\n  mtime = toUnixTimestamp(mtime);\n  binding.futimes(fd, atime, mtime);\n};\n\nfunction writeAll(fd, buffer, offset, length, callback) {\n  // write(fd, buffer, offset, length, position, callback)\n  fs.write(fd, buffer, offset, length, offset, function(writeErr, written) {\n    if (writeErr) {\n      fs.close(fd, function() {\n        if (callback) callback(writeErr);\n      });\n    } else {\n      if (written === length) {\n        fs.close(fd, callback);\n      } else {\n        writeAll(fd, buffer, offset + written, length - written, callback);\n      }\n    }\n  });\n}\n\nfs.writeFile = function(path, data, encoding_, callback) {\n  var encoding = (typeof(encoding_) == 'string' ? encoding_ : 'utf8');\n  var callback_ = arguments[arguments.length - 1];\n  callback = (typeof(callback_) == 'function' ? callback_ : null);\n  fs.open(path, 'w', 438 /*=0666*/, function(openErr, fd) {\n    if (openErr) {\n      if (callback) callback(openErr);\n    } else {\n      var buffer = Buffer.isBuffer(data) ? data : new Buffer('' + data,\n          encoding);\n      writeAll(fd, buffer, 0, buffer.length, callback);\n    }\n  });\n};\n\nfs.writeFileSync = function(path, data, encoding) {\n  var fd = fs.openSync(path, 'w');\n  if (!Buffer.isBuffer(data)) {\n    data = new Buffer('' + data, encoding || 'utf8');\n  }\n  var written = 0;\n  var length = data.length;\n  //writeSync(fd, buffer, offset, length, position)\n  while (written < length) {\n    written += fs.writeSync(fd, data, written, length - written, written);\n  }\n  fs.closeSync(fd);\n};\n\n\nfunction errnoException(errorno, syscall) {\n  // TODO make this more compatible with ErrnoException from src/node.cc\n  // Once all of Node is using this function the ErrnoException from\n  // src/node.cc should be removed.\n  var e = new Error(syscall + ' ' + errorno);\n  e.errno = e.code = errorno;\n  e.syscall = syscall;\n  return e;\n}\n\n\nfunction FSWatcher() {\n  var self = this;\n  var FSEvent = process.binding('fs_event_wrap').FSEvent;\n  this._handle = new FSEvent();\n\n  this._handle.onchange = function(status, event, filename) {\n    if (status) {\n      self.emit('error', errnoException(errno, 'watch'));\n    } else {\n      self.emit('change', event, filename);\n    }\n  };\n}\nutil.inherits(FSWatcher, EventEmitter);\n\nFSWatcher.prototype.start = function(filename, persistent) {\n  var r = this._handle.start(pathModule._makeLong(filename), persistent);\n\n  if (r) {\n    this._handle.close();\n    throw errnoException(errno, 'watch');\n  }\n};\n\nFSWatcher.prototype.close = function() {\n  this._handle.close();\n};\n\nfs.watch = function(filename) {\n  var watcher;\n  var options;\n  var listener;\n\n  if ('object' == typeof arguments[1]) {\n    options = arguments[1];\n    listener = arguments[2];\n  } else {\n    options = {};\n    listener = arguments[1];\n  }\n\n  if (!listener) {\n    throw new Error('watch requires a listener function');\n  }\n\n  if (options.persistent === undefined) options.persistent = true;\n\n  watcher = new FSWatcher();\n  watcher.start(filename, options.persistent);\n\n  watcher.addListener('change', listener);\n  return watcher;\n};\n\n\n// Stat Change Watchers\n\nfunction StatWatcher() {\n  var self = this;\n  this._handle = new binding.StatWatcher();\n\n  this._handle.onchange = function(current, previous) {\n    self.emit('change', current, previous);\n  };\n\n  this._handle.onstop = function() {\n    self.emit('stop');\n  };\n}\nutil.inherits(StatWatcher, EventEmitter);\n\n\nStatWatcher.prototype.start = function(filename, persistent, interval) {\n  this._handle.start(pathModule._makeLong(filename), persistent, interval);\n};\n\n\nStatWatcher.prototype.stop = function() {\n  this._handle.stop();\n};\n\n\nvar statWatchers = {};\nfunction inStatWatchers(filename) {\n  return Object.prototype.hasOwnProperty.call(statWatchers, filename) &&\n      statWatchers[filename];\n}\n\n\nfs.watchFile = function(filename) {\n  if (isWindows) {\n    throw new Error('use fs.watch api instead');\n  }\n\n  var stat;\n  var options;\n  var listener;\n\n  if ('object' == typeof arguments[1]) {\n    options = arguments[1];\n    listener = arguments[2];\n  } else {\n    options = {};\n    listener = arguments[1];\n  }\n\n  if (!listener) {\n    throw new Error('watchFile requires a listener function');\n  }\n\n  if (options.persistent === undefined) options.persistent = true;\n  if (options.interval === undefined) options.interval = 0;\n\n  if (inStatWatchers(filename)) {\n    stat = statWatchers[filename];\n  } else {\n    stat = statWatchers[filename] = new StatWatcher();\n    stat.start(filename, options.persistent, options.interval);\n  }\n  stat.addListener('change', listener);\n  return stat;\n};\n\nfs.unwatchFile = function(filename) {\n  var stat;\n  if (inStatWatchers(filename)) {\n    stat = statWatchers[filename];\n    stat.stop();\n    statWatchers[filename] = undefined;\n  }\n};\n\n// Realpath\n// Not using realpath(2) because it's bad.\n// See: http://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html\n\nvar normalize = pathModule.normalize,\n    isWindows = process.platform === 'win32';\n\nif (isWindows) {\n  // Node doesn't support symlinks / lstat on windows. Hence realpath is just\n  // the same as path.resolve that fails if the path doesn't exists.\n\n  // windows version\n  fs.realpathSync = function realpathSync(p, cache) {\n    p = pathModule.resolve(p);\n    if (cache && Object.prototype.hasOwnProperty.call(cache, p)) {\n      return cache[p];\n    }\n    fs.statSync(p);\n    if (cache) cache[p] = p;\n    return p;\n  };\n\n  // windows version\n  fs.realpath = function(p, cache, cb) {\n    if (typeof cb !== 'function') {\n      cb = cache;\n      cache = null;\n    }\n    p = pathModule.resolve(p);\n    if (cache && Object.prototype.hasOwnProperty.call(cache, p)) {\n      return cb(null, cache[p]);\n    }\n    fs.stat(p, function(err) {\n      if (err) return cb(err);\n      if (cache) cache[p] = p;\n      cb(null, p);\n    });\n  };\n\n\n} else /* posix */ {\n\n  // Regexp that finds the next partion of a (partial) path\n  // result is [base_with_slash, base], e.g. ['somedir/', 'somedir']\n  var nextPartRe = /(.*?)(?:[\\/]+|$)/g;\n\n  // posix version\n  fs.realpathSync = function realpathSync(p, cache) {\n    // make p is absolute\n    p = pathModule.resolve(p);\n\n    if (cache && Object.prototype.hasOwnProperty.call(cache, p)) {\n      return cache[p];\n    }\n\n    var original = p,\n        seenLinks = {},\n        knownHard = {};\n\n    // current character position in p\n    var pos = 0;\n    // the partial path so far, including a trailing slash if any\n    var current = '';\n    // the partial path without a trailing slash\n    var base = '';\n    // the partial path scanned in the previous round, with slash\n    var previous = '';\n\n    // walk down the path, swapping out linked pathparts for their real\n    // values\n    // NB: p.length changes.\n    while (pos < p.length) {\n      // find the next part\n      nextPartRe.lastIndex = pos;\n      var result = nextPartRe.exec(p);\n      previous = current;\n      current += result[0];\n      base = previous + result[1];\n      pos = nextPartRe.lastIndex;\n\n      // continue if not a symlink, or if root\n      if (!base || knownHard[base] || (cache && cache[base] === base)) {\n        continue;\n      }\n\n      var resolvedLink;\n      if (cache && Object.prototype.hasOwnProperty.call(cache, base)) {\n        // some known symbolic link.  no need to stat again.\n        resolvedLink = cache[base];\n      } else {\n        var stat = fs.lstatSync(base);\n        if (!stat.isSymbolicLink()) {\n          knownHard[base] = true;\n          if (cache) cache[base] = base;\n          continue;\n        }\n\n        // read the link if it wasn't read before\n        var id = stat.dev.toString(32) + ':' + stat.ino.toString(32);\n        if (!seenLinks[id]) {\n          fs.statSync(base);\n          seenLinks[id] = fs.readlinkSync(base);\n          resolvedLink = pathModule.resolve(previous, seenLinks[id]);\n          // track this, if given a cache.\n          if (cache) cache[base] = resolvedLink;\n        }\n      }\n\n      // resolve the link, then start over\n      p = pathModule.resolve(resolvedLink, p.slice(pos));\n      pos = 0;\n      previous = base = current = '';\n    }\n\n    if (cache) cache[original] = p;\n\n    return p;\n  };\n\n\n  // posix version\n  fs.realpath = function realpath(p, cache, cb) {\n    if (typeof cb !== 'function') {\n      cb = cache;\n      cache = null;\n    }\n\n    // make p is absolute\n    p = pathModule.resolve(p);\n\n    if (cache && Object.prototype.hasOwnProperty.call(cache, p)) {\n      return cb(null, cache[p]);\n    }\n\n    var original = p,\n        seenLinks = {},\n        knownHard = {};\n\n    // current character position in p\n    var pos = 0;\n    // the partial path so far, including a trailing slash if any\n    var current = '';\n    // the partial path without a trailing slash\n    var base = '';\n    // the partial path scanned in the previous round, with slash\n    var previous = '';\n\n    // walk down the path, swapping out linked pathparts for their real\n    // values\n    LOOP();\n    function LOOP() {\n      // stop if scanned past end of path\n      if (pos >= p.length) {\n        if (cache) cache[original] = p;\n        return cb(null, p);\n      }\n\n      // find the next part\n      nextPartRe.lastIndex = pos;\n      var result = nextPartRe.exec(p);\n      previous = current;\n      current += result[0];\n      base = previous + result[1];\n      pos = nextPartRe.lastIndex;\n\n      // continue if known to be hard or if root or in cache already.\n      if (!base || knownHard[base] || (cache && cache[base] === base)) {\n        return process.nextTick(LOOP);\n      }\n\n      if (cache && Object.prototype.hasOwnProperty.call(cache, base)) {\n        // known symbolic link.  no need to stat again.\n        return gotResolvedLink(cache[base]);\n      }\n\n      return fs.lstat(base, gotStat);\n    }\n\n    function gotStat(err, stat) {\n      if (err) return cb(err);\n\n      // if not a symlink, skip to the next path part\n      if (!stat.isSymbolicLink()) {\n        knownHard[base] = true;\n        if (cache) cache[base] = base;\n        return process.nextTick(LOOP);\n      }\n\n      // stat & read the link if not read before\n      // call gotTarget as soon as the link target is known\n      var id = stat.dev.toString(32) + ':' + stat.ino.toString(32);\n      if (seenLinks[id]) {\n        return gotTarget(null, seenLinks[id], base);\n      }\n      fs.stat(base, function(err) {\n        if (err) return cb(err);\n\n        fs.readlink(base, function(err, target) {\n          gotTarget(err, seenLinks[id] = target);\n        });\n      });\n    }\n\n    function gotTarget(err, target, base) {\n      if (err) return cb(err);\n\n      var resolvedLink = pathModule.resolve(previous, target);\n      if (cache) cache[base] = resolvedLink;\n      gotResolvedLink(resolvedLink);\n    }\n\n    function gotResolvedLink(resolvedLink) {\n\n      // resolve the link, then start over\n      p = pathModule.resolve(resolvedLink, p.slice(pos));\n      pos = 0;\n      previous = base = current = '';\n\n      return process.nextTick(LOOP);\n    }\n  };\n\n}\n\n\nvar pool;\n\nfunction allocNewPool() {\n  pool = new Buffer(kPoolSize);\n  pool.used = 0;\n}\n\n\n\nfs.createReadStream = function(path, options) {\n  return new ReadStream(path, options);\n};\n\nvar ReadStream = fs.ReadStream = function(path, options) {\n  if (!(this instanceof ReadStream)) return new ReadStream(path, options);\n\n  Stream.call(this);\n\n  var self = this;\n\n  this.path = path;\n  this.fd = null;\n  this.readable = true;\n  this.paused = false;\n\n  this.flags = 'r';\n  this.mode = 438; /*=0666*/\n  this.bufferSize = 64 * 1024;\n\n  options = options || {};\n\n  // Mixin options into this\n  var keys = Object.keys(options);\n  for (var index = 0, length = keys.length; index < length; index++) {\n    var key = keys[index];\n    this[key] = options[key];\n  }\n\n  if (this.encoding) this.setEncoding(this.encoding);\n\n  if (this.start !== undefined) {\n    if (this.end === undefined) {\n      this.end = Infinity;\n    }\n\n    if (this.start > this.end) {\n      throw new Error('start must be <= end');\n    }\n\n    this.pos = this.start;\n  }\n\n  if (this.fd !== null) {\n    return;\n  }\n\n  fs.open(this.path, this.flags, this.mode, function(err, fd) {\n    if (err) {\n      self.emit('error', err);\n      self.readable = false;\n      return;\n    }\n\n    self.fd = fd;\n    self.emit('open', fd);\n    self._read();\n  });\n};\nutil.inherits(ReadStream, Stream);\n\nfs.FileReadStream = fs.ReadStream; // support the legacy name\n\nReadStream.prototype.setEncoding = function(encoding) {\n  var StringDecoder = require('string_decoder').StringDecoder; // lazy load\n  this._decoder = new StringDecoder(encoding);\n};\n\n\nReadStream.prototype._read = function() {\n  var self = this;\n  if (!this.readable || this.paused || this.reading) return;\n\n  this.reading = true;\n\n  if (!pool || pool.length - pool.used < kMinPoolSpace) {\n    // discard the old pool. Can't add to the free list because\n    // users might have refernces to slices on it.\n    pool = null;\n    allocNewPool();\n  }\n\n  // Grab another reference to the pool in the case that while we're in the\n  // thread pool another read() finishes up the pool, and allocates a new\n  // one.\n  var thisPool = pool;\n  var toRead = Math.min(pool.length - pool.used, this.bufferSize);\n  var start = pool.used;\n\n  if (this.pos !== undefined) {\n    toRead = Math.min(this.end - this.pos + 1, toRead);\n  }\n\n  function afterRead(err, bytesRead) {\n    self.reading = false;\n    if (err) {\n      self.emit('error', err);\n      self.readable = false;\n      return;\n    }\n\n    if (bytesRead === 0) {\n      self.emit('end');\n      self.destroy();\n      return;\n    }\n\n    var b = thisPool.slice(start, start + bytesRead);\n\n    // Possible optimizition here?\n    // Reclaim some bytes if bytesRead < toRead?\n    // Would need to ensure that pool === thisPool.\n\n    // do not emit events if the stream is paused\n    if (self.paused) {\n      self.buffer = b;\n      return;\n    }\n\n    // do not emit events anymore after we declared the stream unreadable\n    if (!self.readable) return;\n\n    self._emitData(b);\n    self._read();\n  }\n\n  fs.read(this.fd, pool, pool.used, toRead, this.pos, afterRead);\n\n  if (this.pos !== undefined) {\n    this.pos += toRead;\n  }\n  pool.used += toRead;\n};\n\n\nReadStream.prototype._emitData = function(d) {\n  if (this._decoder) {\n    var string = this._decoder.write(d);\n    if (string.length) this.emit('data', string);\n  } else {\n    this.emit('data', d);\n  }\n};\n\n\nReadStream.prototype.destroy = function(cb) {\n  var self = this;\n  this.readable = false;\n\n  function close() {\n    fs.close(self.fd, function(err) {\n      if (err) {\n        if (cb) cb(err);\n        self.emit('error', err);\n        return;\n      }\n\n      if (cb) cb(null);\n      self.emit('close');\n    });\n  }\n\n  if (this.fd) {\n    close();\n  } else {\n    this.addListener('open', close);\n  }\n};\n\n\nReadStream.prototype.pause = function() {\n  this.paused = true;\n};\n\n\nReadStream.prototype.resume = function() {\n  this.paused = false;\n\n  if (this.buffer) {\n    this._emitData(this.buffer);\n    this.buffer = null;\n  }\n\n  // hasn't opened yet.\n  if (null == this.fd) return;\n\n  this._read();\n};\n\n\n\nfs.createWriteStream = function(path, options) {\n  return new WriteStream(path, options);\n};\n\nvar WriteStream = fs.WriteStream = function(path, options) {\n  if (!(this instanceof WriteStream)) return new WriteStream(path, options);\n\n  Stream.call(this);\n\n  this.path = path;\n  this.fd = null;\n  this.writable = true;\n\n  this.flags = 'w';\n  this.encoding = 'binary';\n  this.mode = 438; /*=0666*/\n  this.bytesWritten = 0;\n\n  options = options || {};\n\n  // Mixin options into this\n  var keys = Object.keys(options);\n  for (var index = 0, length = keys.length; index < length; index++) {\n    var key = keys[index];\n    this[key] = options[key];\n  }\n\n  if (this.start !== undefined) {\n    if (this.start < 0) {\n      throw new Error('start must be >= zero');\n    }\n\n    this.pos = this.start;\n  }\n\n  this.busy = false;\n  this._queue = [];\n\n  if (this.fd === null) {\n    this._queue.push([fs.open, this.path, this.flags, this.mode, undefined]);\n    this.flush();\n  }\n};\nutil.inherits(WriteStream, Stream);\n\nfs.FileWriteStream = fs.WriteStream; // support the legacy name\n\nWriteStream.prototype.flush = function() {\n  if (this.busy) return;\n  var self = this;\n\n  var args = this._queue.shift();\n  if (!args) {\n    if (this.drainable) { this.emit('drain'); }\n    return;\n  }\n\n  this.busy = true;\n\n  var method = args.shift(),\n      cb = args.pop();\n\n  args.push(function(err) {\n    self.busy = false;\n\n    if (err) {\n      self.writable = false;\n      if (cb) {\n        cb(err);\n      }\n      self.emit('error', err);\n      return;\n    }\n\n    if (method == fs.write) {\n      self.bytesWritten += arguments[1];\n      if (cb) {\n        // write callback\n        cb(null, arguments[1]);\n      }\n\n    } else if (method === fs.open) {\n      // save reference for file pointer\n      self.fd = arguments[1];\n      self.emit('open', self.fd);\n\n    } else if (method === fs.close) {\n      // stop flushing after close\n      if (cb) {\n        cb(null);\n      }\n      self.emit('close');\n      return;\n    }\n\n    self.flush();\n  });\n\n  // Inject the file pointer\n  if (method !== fs.open) {\n    args.unshift(this.fd);\n  }\n\n  method.apply(this, args);\n};\n\nWriteStream.prototype.write = function(data) {\n  if (!this.writable) {\n    this.emit('error', new Error('stream not writable'));\n    return false;\n  }\n\n  this.drainable = true;\n\n  var cb;\n  if (typeof(arguments[arguments.length - 1]) == 'function') {\n    cb = arguments[arguments.length - 1];\n  }\n\n  if (!Buffer.isBuffer(data)) {\n    var encoding = 'utf8';\n    if (typeof(arguments[1]) == 'string') encoding = arguments[1];\n    data = new Buffer('' + data, encoding);\n  }\n\n  this._queue.push([fs.write, data, 0, data.length, this.pos, cb]);\n\n  if (this.pos !== undefined) {\n    this.pos += data.length;\n  }\n\n  this.flush();\n\n  return false;\n};\n\nWriteStream.prototype.end = function(data, encoding, cb) {\n  if (typeof(data) === 'function') {\n    cb = data;\n  } else if (typeof(encoding) === 'function') {\n    cb = encoding;\n    this.write(data);\n  } else if (arguments.length > 0) {\n    this.write(data, encoding);\n  }\n  this.writable = false;\n  this._queue.push([fs.close, cb]);\n  this.flush();\n};\n\nWriteStream.prototype.destroy = function(cb) {\n  var self = this;\n  this.writable = false;\n\n  function close() {\n    fs.close(self.fd, function(err) {\n      if (err) {\n        if (cb) { cb(err); }\n        self.emit('error', err);\n        return;\n      }\n\n      if (cb) { cb(null); }\n      self.emit('close');\n    });\n  }\n\n  if (this.fd) {\n    close();\n  } else {\n    this.addListener('open', close);\n  }\n};\n\n// There is no shutdown() for files.\nWriteStream.prototype.destroySoon = WriteStream.prototype.end;\n\n\n// SyncWriteStream is internal. DO NOT USE.\n// Temporary hack for process.stdout and process.stderr when piped to files.\nfunction SyncWriteStream(fd) {\n  this.fd = fd;\n  this.writable = true;\n  this.readable = false;\n};\nutil.inherits(SyncWriteStream, Stream);\n\n\n// Export\nfs.SyncWriteStream = SyncWriteStream;\n\n\nSyncWriteStream.prototype.write = function(data, arg1, arg2) {\n  var encoding, cb;\n\n  // parse arguments\n  if (arg1) {\n    if (typeof arg1 === 'string') {\n      encoding = arg1;\n      cb = arg2;\n    } else if (typeof arg1 === 'function') {\n      cb = arg1;\n    } else {\n      throw new Error(\"bad arg\");\n    }\n  }\n\n  // Change strings to buffers. SLOW\n  if (typeof data == 'string') {\n    data = new Buffer(data, encoding);\n  }\n\n  fs.writeSync(this.fd, data, 0, data.length);\n\n  if (cb) {\n    process.nextTick(cb);\n  }\n\n  return true;\n};\n\n\nSyncWriteStream.prototype.end = function(data, arg1, arg2) {\n  if (data) {\n    this.write(data, arg1, arg2);\n  }\n  this.destroy();\n};\n\n\nSyncWriteStream.prototype.destroy = function() {\n  fs.closeSync(this.fd);\n  this.fd = null;\n  this.emit('close');\n  return true;\n};\n\nSyncWriteStream.prototype.destroySoon = SyncWriteStream.prototype.destroy;\n//END lib/fs.js\n    })", "gear:fs")(exports, require, module, binding);
}
static NativeModule _module_fs("fs", _setup_fs);
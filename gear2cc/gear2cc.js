try {var codeaze = load("Codeaze.js");}catch(e){var codeaze = load("gear2cc/Codeaze.js");} /** @todo require("./Codeaze.js") */
var $aze = new codeaze.Codeaze();


function createClass(name, childs, line, inheritList) {
    var out = {type:'class', name:name, inheritList:inheritList, classes:{}, vars:{}, staticVars:{}, functions:{}, staticFunctions:{}, accessors:{}};
    for(c in childs) {
        var node = childs[c];
        switch(node.type) {
            case 'class':
                out.classes[node.name] = {classes:node.classes, vars:node.vars, staticVars:node.staticVars, functions:node.functions, staticFunctions:node.staticFunctions, accessors:node.accessors};
                break;
            case 'function':
                if(!out.functions[node.name])
                    out.functions[node.name] = [{args:node.args, code:node.code, line:node.line}];
                else
                    out.functions[node.name].push({args:node.args, code:node.code, line:node.line});
                break;
            case 'static-function':
                if(!out.staticFunctions[node.name])
                    out.staticFunctions[node.name] = [{args:node.args, code:node.code, line:node.line}];
                else
                    out.staticFunctions[node.name].push({args:node.args, code:node.code, line:node.line});
                break;
            case 'var':
                out.vars[node.name] = {val:node.val};
                break;
            case 'static-var':
                out.staticVars[node.name] = {val:node.val};
                break;
            case 'getter':
                if(!out.accessors[node.name])
                    out.accessors[node.name] = {};
                out.accessors[node.name].getter = {args:node.args, code:node.code, line:node.line};
                break;
            case 'setter':
                if(!out.accessors[node.name])
                    out.accessors[node.name] = {};
                out.accessors[node.name].setter = {args:node.args, code:node.code, line:node.line};
                break;
            case 'native-block':
                throw Error('TODO: Native blocks in classes');
        }
    }
    
    if(!out.functions.hasOwnProperty(name))
        out.functions[name] = [{args:[], code:'', line:line}];
    
    return out;
}

function createObject(name, childs, line, isModule) {
    var out = {type:isModule?'module':'object', name:name, objects:{}, modules:{}, classes:{}, vars:{}, functions:{}, header:'', top:'', license:'', postSetup:''};
    for(c in childs) {
        var node = childs[c];
        switch(node.type) {
            case 'object':
                out.objects[node.name] = {objects:node.objects, classes:node.classes, vars:node.vars, functions:node.functions, header:node.header};
                break;
            case 'module':
                out.modules[node.name] = {objects:node.objects, classes:node.classes, vars:node.vars, functions:node.functions, header:node.header, postSetup:node.postSetup};
                break;
            case 'class':
                if(node.inheritList) {
                    var baseNode = {classes:{}, vars:{}, staticVars:{}, functions:{}, staticFunctions:{}, accessors:{}};
                    function extend(x, nn) {
                        for(var j in x)
                            if(j in baseNode)
                                for(var k in x[j])
                                    if(!(j == 'functions' && k == nn))
                                        baseNode[j][k] = x[j][k];
                    }
                    for(var i = 0; i < node.inheritList.length; i++)
                        extend(out.classes[node.inheritList[i]], node.inheritList[i]);
                    extend(node);
                    out.classes[node.name] = baseNode;
                }
                else
                    out.classes[node.name] = {classes:node.classes, vars:node.vars, staticVars:node.staticVars, functions:node.functions, staticFunctions:node.staticFunctions, accessors:node.accessors};
                break;
            case 'function':
                if(!out.functions.hasOwnProperty(node.name))
                    out.functions[node.name] = [{args:node.args, code:node.code, line:node.line}];
                else
                    out.functions[node.name].push({args:node.args, code:node.code, line:node.line});
                break;
            case 'var':
                out.vars[node.name] = {val:node.val};
                break;
            case 'native-block':
                if(node.which == 'header')
                    out.header += node.code + '\n';
                else if(node.which == 'top')
                    out.top += node.code + '\n';
                else if(node.which == 'license')
                    out.license += node.code + '\n';
                else if(isModule && node.which == 'postSetup')
                    out.postSetup += node.code + '\n';
                else
                    throw Error('TODO: Native block `' + node.which + '`');
        }
    }
    
    if(!out.functions.hasOwnProperty('toString'))
        out.functions['toString'] = [{args:[], code:'return String("['+(isModule?'module':'object')+' '+name+']");', line:line}];
    
    return out;
}

function nLines(s){return s.replace(/[^\n]/g,'').length;}
function nCols(s){return s.replace(/^(.*\n)+/,'').length;}

function sum(a,b) {
    return a+b;
}

var l = 1;
$aze.symbols.space = new codeaze.Symbol('/\\s/|/\\/\\*.*?\\*\\//', function($){var $$;l+=nLines($[0][0]);$$=$[0][0];return $$;});
$aze.symbols.l = new codeaze.Symbol(/(?:)/, function($){var $$;$$=l;return $$;});
$aze.symbols.identifier = new codeaze.Symbol(/~?[A-Za-z_]\w*/, function($){var $$;$$=$[0];return $$;});
$aze.symbols.identifierList = new codeaze.Symbol('(identifier( /,/ identifier)*)?', function($){var $$;$$=$[0]?[$[0][0]].concat($[0][1].map(function(x){return x[3]})):[];return $$;});
$aze.symbols.bulkCode = new codeaze.Symbol(/[^;{}()]+/, function($){var $$;l+=nLines($[0]);$$=$[0];return $$;});
$aze.symbols.nativeCodeInline = new codeaze.Symbol('(bulkCode|/\\(/nativeCodeInline/\\)/)*', function($){var $$;$$=$[0].map(function(x){return x.length>1?x[0][0]+x[1]+x[2][0]:x[0]}).reduce(sum,'');return $$;});
$aze.symbols.nativeCode = new codeaze.Symbol('(bulkCode|/;/|/{/nativeCode/}/|/\\(/nativeCode/\\)/)*', function($){var $$;$$=$[0].map(function(x){return x.length>1?x[0][0]+x[1]+x[2][0]:(x[0][0]==';'?';':x[0])}).reduce(sum,'');return $$;});
$aze.symbols.module = new codeaze.Symbol('l/module/ +identifier /{/ objectContents /}/', function($){var $$;$$=createObject($[3], $[7], $[0], true);return $$;});
$aze.symbols.object = new codeaze.Symbol('l/object/ +identifier /{/ objectContents /}/', function($){var $$;$$=createObject($[3], $[7], $[0], false);return $$;});
$aze.symbols.class = new codeaze.Symbol('l/class/ +identifier( /:/ identifierList)? /{/ objectContents /}/', function($){var $$;$$=createClass($[3], $[8], $[0], $[4]&&$[4][3]);return $$;});
$aze.symbols.variableDef = new codeaze.Symbol('l(/static/ )?/var/ +identifier /=/ nativeCodeInline /;/', function($){var $$;$$={type:$[1]?'static-var':'var', name:$[4], val:$[8]};return $$;});
$aze.symbols.function = new codeaze.Symbol('l(/static/ )?/function/ +identifier /\\(/ identifierList /\\)/ /{/nativeCode/}/', function($){var $$;$$={type:$[1]?'static-function':'function', name:$[4], args:$[8], code:$[13], line:$[0]};return $$;});
$aze.symbols.getter = new codeaze.Symbol('l/get/ +identifier /\\(/ /\\)/ /{/nativeCode/}/', function($){var $$;$$={type:'getter', name:$[3], args:[], code:$[10], line:$[0]};return $$;});
$aze.symbols.setter = new codeaze.Symbol('l/set/ +identifier /\\(/ identifier /\\)/ /{/nativeCode/}/', function($){var $$;$$={type:'setter', name:$[3], args:[$[7]], code:$[12], line:$[0]};return $$;});
$aze.symbols.nativeBlock = new codeaze.Symbol('l,identifier /{/nativeCode/}/', function($){var $$;$$={type:'native-block', which:$[1], code:$[4], line:$[0]};return $$;});
$aze.symbols.objectContents = new codeaze.Symbol('( +|object|class|variableDef|function|getter|setter|nativeBlock)*', function($){var $$;$$=$[0].map(function(x){return x[0]}).filter(function(x){return !x.length});return $$;});
$aze.symbols.main = new codeaze.Symbol('( +|module|nativeBlock)*', function($){var $$;$$=$[0].map(function(x){return x[0]}).filter(function(x){return !x.length});return $$;});


function makeTabs(n, ch) {
    var s = '';
    for(var i = 0; i < n; i++)
        s += ch;
    return s;
}

function makeLine(tbs, line) {
    return '\n' + tbs + '#line ' + line + ' "' + gear.gear + '"';
}

var lineNumber = 1;
function generateFunctionCode(functions, name, parentPrefix, parentPath, code, _class, ctor, dest) {
    var prefix = parentPrefix + '_' + name, path = parentPath + '["' + name + '"]', replaces = [], funcCode = '', hasNoArgsVer = false;
    functions.sort(function(a, b) {return b.args.length - a.args.length;});
    for(f in functions) {
        var func = functions[f], replaces = [], tbs = (!dest && func.args.length ? '\t\t' : '\t');
        var actualCode = '\n' + tbs + func.code.trim() + '\n';
        
        var argsLine = '';
        if(dest=='setter')
            argsLine = func.args[0]+'(_'+func.args[0]+')';
        else
            for(var _arg in func.args)
                argsLine += (argsLine ? ', ' : '') + func.args[_arg] + '(args[' + _arg + '])';
        if(argsLine)
            actualCode = makeLine(tbs, func.line) + '\n' + tbs + 'Value ' + argsLine + ';' + actualCode;
        else
            actualCode = makeLine(tbs, func.line + 1) + actualCode;
        
        replaces.push({regex:'\n' + makeTabs(prefix.split('_').length-1, '    '), replace:'\n' + tbs});
        if(dest!='setter')
            replaces.push({regex:'\\breturn\\b\\s*;', replace:'return undefined;'});
        replaces.push({regex:'\\bthis\\b', replace:'This'});
        
        for(r in replaces) {
            var replace = replaces[r];
            actualCode = actualCode.replace(new RegExp(replace.regex, 'g'), replace.replace);
        }
        if(dest!='setter' && !RegExp('\n'+tbs+'\\breturn\\b[^;]*;\\s*$').exec(actualCode))
            actualCode += tbs + 'return undefined;\n';
        
        if(!dest && func.args.length)
            funcCode += '\n\tif(args.Length() >= ' + func.args.length + ') {' + actualCode + '\t}\n';
        else {
            funcCode += actualCode;
            hasNoArgsVer = true;
        }
    }
    
    if(_class)
        funcCode = '\n\tValue This(args.This());'+funcCode;
    
    if(!hasNoArgsVer)
        funcCode += '\tTHROW_ERROR("Invalid call to ' + parentPrefix.replace(/_/g, '.').replace(/^\./, '') + (ctor ? '' : (_class?'.prototype':'') + '.' + name) + '");\n';
    if(dest=='getter')
        code.func += 'static v8::Handle<v8::Value> ' + prefix + '(v8::Local<v8::String>, const v8::AccessorInfo &args) {' + funcCode + '}\n\n';
    else if(dest=='setter')
        code.func += 'static void ' + prefix + '(v8::Local<v8::String>, v8::Local<v8::Value> _'+func.args[0]+', const v8::AccessorInfo &args) {' + funcCode + '}\n\n';
    else
        code.func += 'static v8::Handle<v8::Value> ' + prefix + '(const v8::Arguments &args) {' + funcCode + '}\n\n';
}

function generateClassCode(_class, name, parentPrefix, parentPath, code) {
    var prefix = parentPrefix + '_' + name, path = parentPath + '["' + name + '"]';
    
    code.addClass(prefix, name);
    
    for(funcName in _class.functions) {
        if(funcName != name)
            code.setPrototype(prefix, funcName, code.makeFunction(prefix + '_' + funcName, funcName));
        generateFunctionCode(_class.functions[funcName], funcName, prefix, prefix, code, _class, funcName == name);
    }
    
    for(accName in _class.accessors) {
        if(!_class.accessors[accName].getter)
            throw new Error('No getter');
        generateFunctionCode([_class.accessors[accName].getter], accName, prefix, prefix, code, _class, false, 'getter');
        if(_class.accessors[accName].setter)
            generateFunctionCode([_class.accessors[accName].setter], accName, prefix, prefix, code, _class, false, 'setter');
        code.setPrototypeAccessor(prefix, accName, prefix + '_' + accName, !!_class.accessors[accName].setter);
    }
    
    for(varName in _class.vars) {
        var val = _class.vars[varName].val;
        code.setPrototype(prefix, varName, /^\s*\b[A-Z]\w+\b\(.+\)$/.test(val) ? val : 'Value(' + val + ')');
    }
    
    code.setStatic(parentPath, name, prefix + '->GetFunction()');
    
    for(className in _class.classes)
        generateClassCode(_class.classes[className], className, prefix, path, code);
    for(varName in _class.staticVars) {
        var val = _class.staticVars[varName].val;
        code.setStatic(path, varName, /^\s*\b[A-Z]\w+\b\(.+\)$/.test(val) ? val : 'Value(' + val + ')');
    }
    for(funcName in _class.staticFunctions) {
        code.setStatic(path, funcName, code.makeFunction(prefix + '_' + funcName, funcName));
        generateFunctionCode(_class.staticFunctions[funcName], funcName, prefix, path, code);
    }
}

function generateObjectCode(object, name, parentPrefix, parentPath, code) {
    var prefix = parentPrefix + '_' + name, path = parentPath + '["' + name + '"]';
    
    for(className in object.classes)
        generateClassCode(object.classes[className], className, prefix, path, code);
    
    for(funcName in object.functions) {
        code.setStatic(path, funcName, code.makeFunction(prefix + '_' + funcName, funcName));
        generateFunctionCode(object.functions[funcName], funcName, prefix, path, code);
    }
    
    for(varName in object.vars) {
        var val = object.vars[varName].val;
        code.setStatic(path, varName, /^\s*\b[A-Z]\w+\b\(.+\)$/.test(val) ? val : 'Value(' + val + ')');
    }
}

function generateModuleCode(object, name, parentPrefix, parentPath, code) {
    var prefix = parentPrefix + '_' + name, path = parentPath;
    
    for(className in object.classes)
        generateClassCode(object.classes[className], className, prefix, path, code);
    
    for(funcName in object.functions) {
        code.setStatic(path, funcName, code.makeFunction(prefix + '_' + funcName, funcName));
        generateFunctionCode(object.functions[funcName], funcName, prefix, path, code);
    }
    
    for(varName in object.vars) {
        var val = object.vars[varName].val;
        code.setStatic(path, varName, /^\s*\b[A-Z]\w+\b\(.+\)$/.test(val) ? val : 'Value(' + val + ')');
    }

    if(object.postSetup)
        code.init += object.postSetup;
}

function generateCode(global) {
    var code = {
        func:'', init:'',
        addObject: function(path) {
            this.init += '\tvar ' + path + ' = Object();\n';
        },
        addClass: function(objName, ctor) {
            this.init += '\tv8::Handle<v8::FunctionTemplate> ' + objName + ' = v8::FunctionTemplate::New(' + objName + '_' + ctor + ');\n';
            this.init += '\t' + objName + '->SetClassName(String("' + ctor + '"));\n';
        },
        makeFunction: function(prefix, name) {
            return 'Function(' + prefix + ', "' + name + '")';
        },
        setStatic: function(parentObjName, name, value) {
            this.init += '\t' + parentObjName + '["' + name + '"] = ' + value + ';\n';
        },
        setPrototype: function(parentObjName, name, value) {
            this.init += '\t' + parentObjName + '->PrototypeTemplate()->Set("' + name + '", ' + value + ');\n';
        },
        setPrototypeAccessor: function(parentObjName, name, getter, setter) {
            this.init += '\t' + parentObjName + '->PrototypeTemplate()->SetAccessor(String("' + name + '"), ' + getter + (setter?', '+getter:'') + ');\n';
        },
    };
    
    var modules = Object.keys(global.modules);
    
    if(!modules.length)
        throw Error('No modules');
    else if(modules.length > 1)
        throw Error('More than one module');
    else {
        var moduleName = modules[0], module = global.modules[moduleName];
        generateModuleCode(module, moduleName, '', '_exports', code);
        
        var license = global.license.trim().replace(/\n    /g, '\n') + (global.license.trim()?'\n\n':'\n'),
            top = global.top.trim().replace(/\n    /g, '\n') + (global.top.trim()?'\n\n':'\n'),
            header = global.header.trim().replace(/\n    /g, '\n') + (global.header.trim()?'\n\n':'\n');
        var ccCode = license+'\
#include <gearbox.h>\n\
#include "'+baseName+'.h"\n\
\n\
using namespace Gearbox;\n\
\n\
/** \\file '+baseName+'.cc converted from '+baseName+'.gear */\n'+
        makeLine('',1) + '\n' + top + code.func;
        
        ccCode += makeLine('',nLines(ccCode)+2).replace('.gear','.cc') + '\nstatic void _setup_' + moduleName + '(Value _exports) {\n' + code.init + '}\nstatic Module _module_' + moduleName + '("'+moduleName+'", _setup_' + moduleName + ');';
        ccCode = ccCode.replace(/\t/g, '    ');
        io.write(gear.cc, ccCode);
        
        var hCode = license+'\
#ifndef GEARBOX_MODULES_'+baseName.toUpperCase()+'_H\n\
#define GEARBOX_MODULES_'+baseName.toUpperCase()+'_H\n\n\
#include <gearbox.h>\n\n'+header+
//void Setup'+baseName+'(v8::Handle<v8::Object> global);\n\n\
'#endif\n';
        io.write(gear.h, hCode);
    }
}

$aze.spaceIgnore = false;
var io = require('Io');
var lastSlash = arguments[1].lastIndexOf('/')+1, baseDir = arguments[1].substr(0, lastSlash), baseName = arguments[1].substr(lastSlash).replace(/\.gear$/, '');
var gear = {gear:baseDir+baseName+'.gear', cc:baseDir+baseName+'.cc', h:baseDir+baseName+'.h'};
if(arguments.length == 2)
    generateCode(createObject('', $aze.parse(io.read(gear.gear)), 1));
else
    print('Usage: ' + arguments[0] + ' <file>');
exit(); // Just in case gearbox-node is a (bit) broken

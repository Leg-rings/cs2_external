---@diagnostic disable-next-line: undefined-global
local ffi = ffi or require "ffi"
local C = ffi.C

---@diagnostic disable
---@format disable-next
local create_interface = (function()ffi.cdef"void* GetModuleHandleA(const char*)"ffi.cdef"void* GetProcAddress(void*, const char*)"local a=ffi.typeof"void*(__cdecl*)(const char*, int*)"return function(b,c)local d=C.GetModuleHandleA(b)if d==nil then return nil end;local e=C.GetProcAddress(d,"CreateInterface")if e==nil then return nil end;local f=ffi.cast(a,e)(c,nil)if f==nil then return nil end;return f end end)()
---@format disable-next
local vtable_bind, vtable_thunk = (function()local a=(function()local b=ffi.typeof"void***"return function(c,d,e)return ffi.cast(e,ffi.cast(b,c)[0][d])end end)()local function f(c,d,e,...)local g=a(c,d,ffi.typeof(e,...))return function(...)return g(c,...)end end;local function h(d,e,...)e=ffi.typeof(e,...)return function(c,...)return a(c,d,e)(c,...)end end;return f,h end)()
---@format disable-next
local find_signature = (function()local a=setmetatable({},{__index=function(b,c)b[c]=ffi.typeof(c)return b[c]end})local function d(e)if ffi.cast(a["uint16_t*"],e)[0]~=0x5A4D then return 0 end;local f=ffi.cast(a["long*"],ffi.cast(a["uintptr_t"],e)+0x003c)[0]local g=ffi.cast(a["uintptr_t"],e)+f;if ffi.cast(a["unsigned long*"],g)[0]~=0x00004550 then return 0 end;return ffi.cast(a["unsigned long*"],g+0x0018+0x0038)[0]end;return function(h,i,j)i=i.."\0"if#i/3==0 or#i%3~=0 then return nil end;local e=C.GetModuleHandleA(h)if e==nil then return nil end;local k=d(e)if k==0 then return nil end;local l={}for m in string.gmatch(i:gsub("%?%?","00"),"(%x%x)")do local n=tonumber(m,16)if not n then return nil end;table.insert(l,n)end;local o=a["uint8_t*"](e)for p=0,k-#l-1 do local q=true;for r=1,#l do local n=l[r]if n~=0 and o[p+r-1]~=n then q=false;break end end;if q then return o+p+(j or 0)end end;return nil end end)()
---@diagnostic enable

local CGameEntitySystem = ffi.cast("void**", ffi.cast("uintptr_t", create_interface("engine2.dll", "GameResourceServiceClientV001")) + 0x58)[0]

local native_GetEntityInstance = (function(instance)
    local fnptr = ffi.cast("void*(__thiscall*)(void*, int)", find_signature("client.dll", "81 FA ?? ?? ?? ?? 77 36 8B C2 C1 F8 09 83 F8 3F 77 2C 48 98"))
    return function(idx) return fnptr(instance, idx) end
end)(CGameEntitySystem)


local reference = gui.Reference("Visuals", "Other", "Effects")
local color_reference = gui.ColorPicker(reference, "skybox.modulation", "Skybox Modulation", 255, 255, 255, 255)

local ts = setmetatable({}, {
    __index = function(t, k)
        t[k] = ffi.typeof(k)
        return t[k]
    end
})

local fnptr = ffi.cast("void*(__thiscall*)(void*)", find_signature("client.dll", "48 8B C4 48 89 58 18 48 89 70 20 55 57 41 54 41 55"))

callbacks.Register("CreateMove", function()
    local r, g, b, a = color_reference:GetValue()
    for _, value in pairs(entities.FindByClass("C_EnvSky")) do
        local instance = native_GetEntityInstance(value:GetIndex())
        if instance == nil then goto continue end

        local p = ffi.cast(ts["uintptr_t"], instance)

        ffi.cast(ts["float*"], p + 0xcec)[0] = math.max(0.01, a / 255)

        local tint_color = ffi.cast(ts["uint8_t*"], p + 0xce1)
        tint_color[0] = r
        tint_color[1] = g
        tint_color[2] = b
        tint_color[3] = a

        local tint_color_lighting_only = ffi.cast(ts["uint8_t*"], p + 0xce5)
        tint_color_lighting_only[0] = r
        tint_color_lighting_only[1] = g
        tint_color_lighting_only[2] = b
        tint_color_lighting_only[3] = a

        fnptr(instance)
        ::continue::
    end
end)
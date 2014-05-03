local filefind = require 'filefind'

local oldNumber, newNumber = ...

function LoadFile(fileName)
	local file = io.open(fileName, "rb")
	local str = file:read("*a")
	file:close()
	return str
end

function SaveFile(fileName, str)
	local file = io.open(fileName, "wb")
	file:write(str)
	file:close()
end

function ReplaceNumbers(wildcard)
	for entry in filefind.glob(wildcard) do
		print(entry.filename)
		local str = LoadFile(entry.filename)
		str = str:gsub(oldNumber, newNumber)
		SaveFile(entry.filename, str)
	end
end

ReplaceNumbers("**.html")
ReplaceNumbers("**.cpp")
ReplaceNumbers("**.h")
ReplaceNumbers("**.rc")


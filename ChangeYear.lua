import "glob"

oldNumber = arg[1]
newNumber = arg[2]

function LoadFile(fileName)
	local file = io.open(fileName, "rt")
	local str = file:read("*a")
	file:close()
	return str
end

function SaveFile(fileName, str)
	local file = io.open(fileName, "wt")
	file:write(str)
	file:close()
end

function ReplaceNumbers(globPath)
	local files = glob.match(globPath)

	for index, fileName in files do
		print(fileName)
		local str = LoadFile(fileName)
		str = str:gsub(oldNumber, newNumber)
		SaveFile(fileName, str)
	end
end

ReplaceNumbers("**.html")
ReplaceNumbers("**.cpp")
ReplaceNumbers("**.h")


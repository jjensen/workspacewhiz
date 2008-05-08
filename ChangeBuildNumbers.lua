import "glob"

oldBuildNumber = arg[1]
newBuildNumber = arg[2]

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

function ReplaceBuildNumbers(globPath)
	local files = glob.match(globPath)

	for index, fileName in files do
		print(fileName)
		local str = LoadFile(fileName)
		str = str:gsub(oldBuildNumber, newBuildNumber)
		SaveFile(fileName, str)
	end
end

ReplaceBuildNumbers("**.rc")
ReplaceBuildNumbers("**.iss")
ReplaceBuildNumbers("**.nsi")
ReplaceBuildNumbers("**.html")


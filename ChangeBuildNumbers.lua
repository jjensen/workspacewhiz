require "glob"

oldBuildNumber, newBuildNumber = unpack(arg)

function ReplaceBuildNumbers(globPath)
	for index, fileName in ipairs(glob.match(globPath)) do
		print(fileName)
		local str = io.readall(fileName)
		str = str:gsub(oldBuildNumber, newBuildNumber)
		io.writeall(fileName, str)
	end
end

ReplaceBuildNumbers("**.rc")
ReplaceBuildNumbers("**.iss")
ReplaceBuildNumbers("**.nsi")
ReplaceBuildNumbers("**.html")


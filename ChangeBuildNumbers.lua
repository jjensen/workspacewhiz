require "glob"

oldBuildNumber, newBuildNumber = unpack(arg)

function ReplaceBuildNumbers(globPath)
	for index, fileName in ipairs(glob.match(globPath)) do
		local str = io.readall(fileName)
		local oldStr = str
		str = str:gsub(oldBuildNumber, newBuildNumber)
		if oldStr ~= str then
			print(fileName)
			io.writeall(fileName, str)
		end
	end
end

ReplaceBuildNumbers("**.rc")
ReplaceBuildNumbers("**.iss")
ReplaceBuildNumbers("**.nsi")
ReplaceBuildNumbers("**.html")


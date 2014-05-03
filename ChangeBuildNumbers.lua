local filefind = require "filefind"

oldBuildNumber, newBuildNumber = unpack(arg)

function ReplaceBuildNumbers(globPath)
	for entry in filefind.glob(globPath) do
		local file = io.open(entry.filename, 'rb')
		local str = file:read('*a')
		file:close()
		local oldStr = str
		str = str:gsub(oldBuildNumber, newBuildNumber)
		if oldStr ~= str then
			print(entry.filename)
			local file = io.open(entry.filename, 'wb')
			file:write(str)
			file:close()
		end
	end
end

ReplaceBuildNumbers("**.rc")
ReplaceBuildNumbers("**.iss")
ReplaceBuildNumbers("**.nsi")
ReplaceBuildNumbers("**.html")


require "filefind"

oldBuildNumber, newBuildNumber = unpack(arg)

function ReplaceBuildNumbers(globPath)
	for entry in filefind.glob(globPath) do
		local str = io.readall(entry.filename)
		local oldStr = str
		str = str:gsub(oldBuildNumber, newBuildNumber)
		if oldStr ~= str then
			print(entry.filename)
			io.writeall(entry.filename, str)
		end
	end
end

ReplaceBuildNumbers("**.rc")
ReplaceBuildNumbers("**.iss")
ReplaceBuildNumbers("**.nsi")
ReplaceBuildNumbers("**.html")


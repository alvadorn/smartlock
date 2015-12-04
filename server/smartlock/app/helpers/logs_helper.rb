module LogsHelper
	def select_icon(status, bt = false)
		title = ""		
		if controller.controller_name == "users" || controller.controller_name == "bluetooth_adapters"
			title = status ? "Activated" : "Deactivated"
		elsif controller.controller_name == "logs"
			if bt
				title = status ? "Bluetooth request" : "Not bluetooth request"
			else
				title = status ? "Authenticated" : "Not Authenticated"
			end
		end
		status ? "<i class=\"fi-check\" title=\"#{title}\"></i>" : "<i class=\"fi-x\" title=\"#{title}\"></i>"
	end

	def text_or_icon(str)
	end
end

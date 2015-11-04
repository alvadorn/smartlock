json.array!(@bluetooth_adapters) do |bluetooth_adapter|
  json.extract! bluetooth_adapter, :id, :token
  json.url bluetooth_adapter_url(bluetooth_adapter, format: :json)
end

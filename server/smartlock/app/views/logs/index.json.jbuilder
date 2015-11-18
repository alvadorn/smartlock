json.array!(@logs) do |log|
  json.extract! log, :id, :bluetooth, :status
  json.url log_url(log, format: :json)
end

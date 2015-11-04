Rails.application.routes.draw do
  post '/auth' => 'authenticate#init'
end

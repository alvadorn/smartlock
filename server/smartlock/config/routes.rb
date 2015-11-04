Rails.application.routes.draw do
  resources :logs, only: [:index, :show]
  resources :users
  resources :bluetooth_adapters

  post '/auth' => 'authenticate#init'
end

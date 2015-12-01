Rails.application.routes.draw do
  resources :logs, only: [:index, :show]
  resources :users do
  end
  resources :bluetooth_adapters
=begin
  namespace :users do
    get '/' => :index
    get '/:id' => :show
    get '/:id/edit' => :edit
    get '/new' => :new
    post '/create' => :create
    put '/:id/activate' => :activate
    put '/:id/deactivate' => :deactivate
  end
=end
  post '/auth' => 'authenticate#init'
  post '/auth/btnew' => 'authenticate#create'
  put '/users/:id/activate' => "users#activate", as: "user_activate"
  put '/users/:id/deactivate' => "users#deactivate", as: "user_deactivate"
  put '/bluetooth_adapters/:id/activate' => "bluetooth_adapters#activate", as: "bluetooth_activate"
  put '/bluetooth_adapters/:id/deactivate' => "bluetooth_adapters#deactivate", as: "bluetooth_deactivate"
end

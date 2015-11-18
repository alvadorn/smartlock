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
  put '/users/:id/activate' => "users#activate", as: "user_activate"
  put '/users/:id/deactivate' => "users#deactivate", as: "user_deactivate"
end

class AuthenticateController < ApplicationController
  skip_before_action :verify_authenticity_token

  def init
    puts params.inspect
  	account = params[:account]
  	password = params[:password]
  	bt = params[:bt]
  	token = params[:token] if bt == "true"
  	pass_hash = password
    if bt == "true"
      query = User.joins(:bluetooth_adapter).where( bluetooth_adapters: {
        token: token }, account: account, password: pass_hash)
      if query.any?
        authorized query[0].name
      else
        unauthorized
      end
    else
      user = User.find_by(account: account, password: pass_hash)
      if user
        authorized user.name
      else
        unauthorized
      end
    end
  end

  def create
    puts params.inspect
    account = params[:account]
    password = params[:password]
    token = params[:token]
    u = User.find_by(account: account, password: password)
    if u
      BluetoothAdapters.create(token: token, user: u, activated: false)
    end
  end


  private
  def unauthorized
  	render json: { authentication: "unauthorized" }
    #{}"{ \"authentication\": \"unauthorized\" }"
  end

  def authorized(name)
  	render json:  { authentication: "authorized", name: name }
    #{}"{ \"authentication\": \"authorized\", \"name\": #{name} }"
  end

end

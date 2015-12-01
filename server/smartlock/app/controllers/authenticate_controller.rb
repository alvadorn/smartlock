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
        token: token, activated: true }, account: account, password: pass_hash, activated: true)
      if query.any?
        puts "achei essa porra"
        authorized query[0].name
        #else
        #  unauthorized
        #end
      else
        unauthorized
      end
    else
      user = User.find_by(account: account, password: pass_hash)
      if user.activated
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
      bt = BluetoothAdapter.new(token: token, user: u, activated: false)
      if bt.save
        success
      else
        fail
      end
    else
      fail
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

  def success
    render json: { create: "success" }
  end

  def fail
    render json: { create: "fail" }
  end

end

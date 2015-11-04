class AuthenticateController < ApplicationController
  skip_before_action :verify_authenticity_token
  def init
  	account = params[:account]
  	password = params[:password]
  	bt = params[:bt]
  	token = params[:token] if bt == "true"
  	pass_hash = password
  	user = User.find_by(account: account, password: pass_hash)
  	if user
  		if bt == "true"
  			bluetooth = BluetoothAdapter.find_by(token: token, user_id: user.id)
  			if bluetooth
  				if bluetooth.activated
  					authorized user.name
  				else
  					unauthorized
  				end
  			else
  				unauthorized
  			end
  		else
  			authorized user.name
  		end
  	end
  end


  private
  def unauthorized
  	render json: "{ \"authentication\": \"unauthorized\" }"
  end

  def authorized(name)
  	render json: "{ \"authentication\": \"authorized\", \"name\": #{name} }"
  end
end

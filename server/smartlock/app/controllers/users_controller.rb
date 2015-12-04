class UsersController < ApplicationController
  before_action :set_user, only: [:show, :edit, :update, :destroy, :deactivate, :activate]
  # GET /users
  # GET /users.json
  def index
    puts params.inspect
    @users = User.all
  end

  # GET /users/1
  # GET /users/1.json
  def show
  end

  # GET /users/new
  def new
    @user = User.new
  end

  # GET /users/1/edit
  def edit
  end

  # POST /users
  # POST /users.json
  def create
    @user = User.new(user_params)

    respond_to do |format|
      if @user.save
        format.html { redirect_to @user, notice: 'User was successfully created.' }
        format.json { render :show, status: :created, location: @user }
      else
        format.html { render :new }
        format.json { render json: @user.errors, status: :unprocessable_entity }
      end
    end
  end

  # PATCH/PUT /users/1
  # PATCH/PUT /users/1.json
  def update
    respond_to do |format|
      if @user.update(user_params)
        format.html { redirect_to @user, notice: 'User was successfully updated.' }
        format.json { render :show, status: :ok, location: @user }
      else
        format.html { render :edit }
        format.json { render json: @user.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /users/1
  # DELETE /users/1.json
  def destroy
    @user.destroy
    respond_to do |format|
      format.html { redirect_to users_url, notice: 'User was successfully destroyed.' }
      format.json { head :no_content }
    end
  end

  # PUT  /users/1/deactivate
  def deactivate
    respond_to do |format|
      if @user.update(activated: false)
        format.html { redirect_to users_url, notice: 'Usuário desativado com sucesso.' }
      else
        format.html { reditect_to users_url, notice: 'Usuário não pode ser desativado' }
      end
    end
  end

  def activate
    respond_to do |format|
      if @user.update(activated: true)
        format.html { redirect_to users_url, notice: 'Usuário ativado com sucesso.' }
      else
        format.html { reditect_to users_url, notice: 'Usuário não pode ser ativado.' }
      end
    end
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_user
      @user = User.find(params[:id])
    end

    # Never trust parameters from the scary internet, only allow the white list through.
    def user_params
      params.require(:user).permit(:name, :surname, :password,:email, :activated, :account)
    end

end

class BluetoothAdaptersController < ApplicationController
  before_action :set_bluetooth_adapter, only: [:show, :edit, :update, :destroy, :activate, :deactivate]

  # GET /bluetooth_adapters
  # GET /bluetooth_adapters.json
  def index
    @bluetooth_adapters = BluetoothAdapter.all
  end

  # GET /bluetooth_adapters/1
  # GET /bluetooth_adapters/1.json
  def show
  end

  # GET /bluetooth_adapters/new
  def new
    @bluetooth_adapter = BluetoothAdapter.new
    @users = User.all
  end

  # GET /bluetooth_adapters/1/edit
  def edit
  end

  # POST /bluetooth_adapters
  # POST /bluetooth_adapters.json
  def create
    @bluetooth_adapter = BluetoothAdapter.new(bluetooth_adapter_params)

    respond_to do |format|
      if @bluetooth_adapter.save
        format.html { redirect_to @bluetooth_adapter, notice: 'Bluetooth adapter was successfully created.' }
        format.json { render :show, status: :created, location: @bluetooth_adapter }
      else
        format.html { render :new }
        format.json { render json: @bluetooth_adapter.errors, status: :unprocessable_entity }
      end
    end
  end

  # PATCH/PUT /bluetooth_adapters/1
  # PATCH/PUT /bluetooth_adapters/1.json
  def update
    respond_to do |format|
      if @bluetooth_adapter.update(bluetooth_adapter_params)
        format.html { redirect_to @bluetooth_adapter, notice: 'Bluetooth adapter was successfully updated.' }
        format.json { render :show, status: :ok, location: @bluetooth_adapter }
      else
        format.html { render :edit }
        format.json { render json: @bluetooth_adapter.errors, status: :unprocessable_entity }
      end
    end
  end

  # DELETE /bluetooth_adapters/1
  # DELETE /bluetooth_adapters/1.json
  def destroy
    @bluetooth_adapter.destroy
    respond_to do |format|
      format.html { redirect_to bluetooth_adapters_url, notice: 'Bluetooth adapter was successfully destroyed.' }
      format.json { head :no_content }
    end
  end

  # PUT  /bluuetooth/1/deactivate
  def deactivate
    respond_to do |format|
      if @bluetooth_adapter.update(activated: false)
        format.html { redirect_to bluetooth_adapters_url, notice: 'Adaptador desativado com sucesso.' }
      else
        format.html { reditect_to bluetooth_adapters_url, notice: 'Adaptador não pode ser desativado' }
      end
    end
  end

  def activate
    respond_to do |format|
      if @bluetooth_adapter.update(activated: true)
        format.html { redirect_to bluetooth_adapters_url, notice: 'Adaptador ativado com sucesso.' }
      else
        format.html { reditect_to bluetooth_adapters_url, notice: 'Adaptador não pode ser ativado.' }
      end
    end
  end

  private
    # Use callbacks to share common setup or constraints between actions.
    def set_bluetooth_adapter
      @bluetooth_adapter = BluetoothAdapter.find(params[:id])
    end

    # Never trust parameters from the scary internet, only allow the white list through.
    def bluetooth_adapter_params
      params.require(:bluetooth_adapter).permit(:token, :user_id)
    end
end

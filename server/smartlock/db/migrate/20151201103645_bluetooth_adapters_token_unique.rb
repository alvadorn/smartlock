class BluetoothAdaptersTokenUnique < ActiveRecord::Migration
  def change
  	change_table :bluetooth_adapters do |bt|
  		add_index :bluetooth_adapters, [ :token ], :unique => true
  	end
  end
end

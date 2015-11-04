class CreateBluetoothAdapters < ActiveRecord::Migration
  def change
    create_table :bluetooth_adapters do |t|
      t.boolean :activated, null: false, default: true
      t.string :token, null: false
      t.belongs_to :user, index: true, null: false

      t.timestamps null: false
    end
  end
end

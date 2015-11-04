class CreateBluetoothAdapters < ActiveRecord::Migration
  def change
    create_table :bluetooth_adapters do |t|
      t.boolean :activated
      t.string :token
      t.belongs_to :user, index: true

      t.timestamps null: false
    end
  end
end

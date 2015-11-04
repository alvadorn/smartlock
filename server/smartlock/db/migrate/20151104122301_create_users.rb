class CreateUsers < ActiveRecord::Migration
  def change
    create_table :users do |t|
      t.string :name, null: false
      t.string :surname, null: false
      t.string :account, unique: true, null: false
      t.string :password, unique: true, null: false
      t.boolean :activated, null: false

      t.timestamps null: false
    end
  end
end

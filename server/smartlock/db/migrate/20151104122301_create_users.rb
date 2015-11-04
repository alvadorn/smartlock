class CreateUsers < ActiveRecord::Migration
  def change
    create_table :users do |t|
      t.string :name, null: false
      t.string :surname, null: false
      t.string :account, unique: true, null: false
      t.string :password, unique: true, null: false
      t.string :email
      t.boolean :activated, null: false, default: true

      t.timestamps null: false
    end
  end
end

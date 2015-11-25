# encoding: UTF-8
# This file is auto-generated from the current state of the database. Instead
# of editing this file, please use the migrations feature of Active Record to
# incrementally modify your database, and then regenerate this schema definition.
#
# Note that this schema.rb definition is the authoritative source for your
# database schema. If you need to create the application database on another
# system, you should be using db:schema:load, not running all the migrations
# from scratch. The latter is a flawed and unsustainable approach (the more migrations
# you'll amass, the slower it'll run and the greater likelihood for issues).
#
# It's strongly recommended that you check this file into your version control system.

ActiveRecord::Schema.define(version: 20151104142924) do

  create_table "bluetooth_adapters", force: :cascade do |t|
    t.boolean  "activated",  default: true, null: false
    t.string   "token",                     null: false
    t.integer  "user_id",                   null: false
    t.datetime "created_at",                null: false
    t.datetime "updated_at",                null: false
  end

  add_index "bluetooth_adapters", ["user_id"], name: "index_bluetooth_adapters_on_user_id"

  create_table "logs", force: :cascade do |t|
    t.boolean  "bluetooth"
    t.boolean  "status"
    t.integer  "user_id"
    t.datetime "created_at", null: false
    t.datetime "updated_at", null: false
  end

  create_table "users", force: :cascade do |t|
    t.string   "name",                      null: false
    t.string   "surname",                   null: false
    t.string   "account",                   null: false
    t.string   "password",                  null: false
    t.string   "email"
    t.boolean  "activated",  default: true, null: false
    t.datetime "created_at",                null: false
    t.datetime "updated_at",                null: false
  end

end

# -*- coding: utf-8 -*-
import random
import config
import telebot
import time

# Create an object of a bot
bot = telebot.TeleBot(config.token)

bot_names = ['Вася', 'вася', 'ВАСЯ', 'Васёк', 'васёк', 'ВАСЁК', 'Васек', 'васек', 'ВАСЕК', 'Васька', 'васька', 'ВАСЬКА',
             'Василий', 'василий', 'ВАСИЛИЙ', 'Васе', 'васе', 'ВАСЕ', 'Васю', 'васю', 'ВАСЮ', 'Васи', 'васи', 'ВАСИ',
             'Вась', 'вась', 'ВАСЬ', 'Ваське', 'ваське', 'ВАСЬКЕ', 'Ваську', 'ваську', 'ВАСЬКУ', 'Василию', 'василию',
             'ВАСИЛИЮ']

hello_words = ['Привет', 'привет', 'ПРИВЕТ', 'Здарова', 'здарова', 'ЗДАРОВА', 'Ку', 'ку', 'КУ', 'Хай', 'хай', 'ХАЙ']

mood_words = ['Как дела', 'как дела', 'КАК ДЕЛА', 'Как настроение', 'как настроение', 'КАК НАСТРОЕНИЕ', 'Чо как',
              'чо как', 'ЧО КАК', 'Как поживаешь', 'как поживаешь', 'КАК ПОЖИВАЕШЬ']

something_words = ['Расскажи', 'скажи', 'скажешь', 'расскажешь', 'Что-нибудь', 'что-нибудь', 'Шо недь', 'шо недь',
                   'Што-нибудь', 'што-нибудь', 'Че-недь', 'че-недь', 'Чё недь', 'чё недь', 'Чо недь', 'чо недь',
                   'ЧО НЕДЬ', 'Шо нибудь', 'шо нибудь', 'ШО НИБУДЬ']

goodbye_words = ['Пока', 'пока', 'ПОКА', 'Споки', 'споки', 'Спокойной ночи', 'спокойной ночи']

swearwords_words = ['Нахуй', 'нахуй', 'Хуй', 'хуй', 'Соси', 'соси', 'Пидор', 'пидор', 'лох', 'Тупой', 'тупой']


# Vasek hello messages
def say_hello(message):
    for word in hello_words:
        if word in message.text:
            rand_val = random.randrange(0, 3)
            if rand_val == 0:
                bot.send_message(message.chat.id, 'здарово')
            elif rand_val == 1:
                bot.send_message(message.chat.id, 'ну привет')
            elif rand_val == 2:
                bot.send_message(message.chat.id, 'чо надо?')
            return 0


# Vasek mood messages
def say_mood(message):
    for word in mood_words:
        if word in message.text:
            rand_val = random.randrange(0, 3)
            if rand_val == 0:
                bot.send_message(message.chat.id, 'нормальна. а че те?')
            elif rand_val == 1:
                bot.send_message(message.chat.id, 'хуёва')
            elif rand_val == 2:
                bot.send_message(message.chat.id, 'зоебись))')
            return 0


# Vasek hello messages
def say_goodbye(message):
    for word in goodbye_words:
        if word in message.text:
            rand_val = random.randrange(0, 3)
            if rand_val == 0:
                bot.send_message(message.chat.id, 'аливидерчи)))')
            elif rand_val == 1:
                bot.send_message(message.chat.id, 'ага')
            elif rand_val == 2:
                bot.send_message(message.chat.id, 'гудбай лошара)')
            return 0


# Vasek respond messages
def say_what(message):
    rand_val = random.randrange(0, 3)
    if rand_val == 0:
        bot.send_message(message.chat.id, 'че?')
    elif rand_val == 1:
        bot.send_message(message.chat.id, 'хули надо')
    elif rand_val == 2:
        bot.send_message(message.chat.id, 'ну?')
    return 0


# Vasek swearword messages
def say_swearword(message):
    for word in swearwords_words:
        if word in message.text:
            rand_val = random.randrange(0, 5)
            if rand_val == 0:
                bot.send_message(message.chat.id, 'иди нахуй сука')
            elif rand_val == 1:
                bot.send_message(message.chat.id, 'саси')
            elif rand_val == 2:
                bot.send_message(message.chat.id, 'ты шлюха тупая отсаси мой хуй')
            elif rand_val == 3:
                bot.send_message(message.chat.id, 'ты сука отсоси мой болт')
            elif rand_val == 4:
                bot.send_message(message.chat.id, 'отсоси мне')
            return 0


# Vasek tells his thoughts
def say_something(message):
    for word in something_words:
        if word in message.text:
            rand_val = random.randrange(0, 10)
            if rand_val == 0:
                bot.send_message(message.chat.id, 'я люблю лерку но она меня не любет')
            elif rand_val == 1:
                bot.send_message(message.chat.id, 'была она с нами теперь ее нет оказалась в кампании алкаголь сигарет')
            elif rand_val == 2:
                bot.send_message(message.chat.id, 'рома пидафил ебаный бородатый черт саси хуй')
            elif rand_val == 3:
                bot.send_message(message.chat.id, 'артем питух')
            elif rand_val == 4:
                bot.send_message(message.chat.id, 'илья лох я те ёбну')
            elif rand_val == 5:
                bot.send_message(message.chat.id, 'хз чё расказать. го на сервак я тя админом сделаю))))))')
            elif rand_val == 6:
                bot.send_message(message.chat.id, 'подпишись https://www.youtube.com/channel/UCV0XK4xmltqmn5tY6by4NbA')
            elif rand_val == 7:
                bot.send_message(message.chat.id, 'када бухать суки на дачю')
            elif rand_val == 8:
                bot.send_message(message.chat.id, 'ниче не хочу говорить иди нахуй')
            elif rand_val == 9:
                bot.send_message(message.chat.id, 'пнх')
            return 0


# Command 'help' handling
@bot.message_handler(commands=['help'])
def help_message(message):
    msg = '''
*************************
****** Vasek Bot v1.0 ******
*************************

 > Vasek не реагирует на сообщения, в которых не упоминается его имя

 > Vasek может не ответить вам вообще, поэтому не стоит на него злиться... но и злить его тоже не стоит :)

 > Vasek может стать лучше благодаря вам, если, конечно, вы предложите идеи по его улучшению

 (с) Eugen Crack
'''
    bot.send_message(message.chat.id, msg)


# Command 'start' handling
@bot.message_handler(commands=['start'])
def start_message(message):
    bot.send_message(message.chat.id, 'всем хай')


# Received message handling
@bot.message_handler(content_types=['text'])
def check_message(message):
    for name in bot_names:
        if name in message.text:
            time.sleep(1)
            # Greetings!
            if say_hello(message) == 0:
                break

            # How r u?
            if say_mood(message) == 0:
                break

            # Say something!
            if say_something(message) == 0:
                break

            # Good bye
            if say_goodbye(message) == 0:
                break

            # Swearwods
            if say_swearword(message) == 0:
                break

            # Bot responds
            if say_what(message) == 0:
                break

if __name__ == '__main__':
    bot.polling(none_stop=True)

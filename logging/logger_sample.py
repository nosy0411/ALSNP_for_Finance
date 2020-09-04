class LogManager:
    def __init__(self, host_name, instance_name, log_level, log_file_name):
        self.host_name     = host_name
        self.instance_name = instance_name
        self.log_level     = log_level
        self.log_file_name = log_file_name
        self.logger_list   = []

    def get_logger(self, logger_name):
        logger = logging.getLogger(logger_name)
        logger.propagate = 0 # 로그가 계속 전파되지 않도록 설정
        logger.setLevel(self.get_loglevel()) # 로그레벨 설정
        logger.addHandler(self.get_stream_handler())
        logger.addHandler(self.get_file_handler())
        self.logger_list.append(logger)
        return logger
        
    def get_loglevel(self):
        # https://docs.python.org/3/library/logging.html#levels 참고
        # 'DEBUG', 'INFO', 'WARNING', 'ERROR', 'CRITICAL'
        return self.log_level
    
    def set_loglevel(self, log_level):
        self.log_level = log_level
        for logger in self.logger_list: # 모든 로거의 레벨을 수정한다
            logger.setLevel(self.get_loglevel()) # 로그레벨 설정            
    
    def get_stream_handler(self):
        stream_handler = logging.StreamHandler()
        stream_handler.setFormatter(self.get_my_log_format())
        return stream_handler
        
    def get_file_handler(self):
        file_handler = logging.FileHandler(self.log_file_name)
        file_handler.setFormatter(self.get_my_log_format())
        return file_handler
    
    def get_my_log_format(self):
        # https://docs.python.org/3/library/logging.html#logging.Formatter 참고
        return logging.Formatter('[%(asctime)s]'                # 일시
                               + '[%(levelname)s]'              # 로그레벨
                               + '[' + self.host_name + ']'     # 호스트명
                               + '[' + self.instance_name + ']' # 인스턴스명
                               + '[%(name)s]'                   # 로거 이름
                               + '[%(funcName)s]'               # 함수명
                               + '- %(message)s')               # 메시지

import logging
import socket

host_name     = socket.gethostname()
instance_name = 'instance1'
log_level     = logging.WARNING # 기본
log_file_name = 'myapp' + '.log'

logManager = LogManager(host_name=host_name, instance_name=instance_name, \
                        log_level=log_level, log_file_name=log_file_name)
print(logManager)
logger = logManager.get_logger('Root')

def hi():
    logger.info('Welcome to Python Log World.')

class MyClass:
    def __init__(self, logManager):
        self.logger = logManager.get_logger(self.__class__.__name__)
    def hello(self):
        self.logger.info('Welcome to Python Class Log World')

myClass = MyClass(logManager)
myClass.hello()
